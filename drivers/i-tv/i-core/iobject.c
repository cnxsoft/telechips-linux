#include <i-tv/itv_common.h>
#include <i-tv/itv_object.h>

typedef struct itv_object_internals_t {
	int i_object_type;

	spinlock_t ref_spin;
	unsigned int refcount;
	itv_destructor_t pf_destructor;

	itv_object_t *prev, *next;
	itv_object_t **pp_children;
	int i_children;
} itv_object_internals_t;

#define itv_object_internals(obj)	(((itv_object_internals_t *)(ITV_OBJECT(obj))) - 1)

static void itv_lock(itv_core_t *p_icore)
{
	spin_lock(&p_icore->structure_lock);
}

static void itv_unlock(itv_core_t *p_icore)
{
	spin_unlock(&p_icore->structure_lock);
}

itv_object_t *__itv_object_hold(itv_object_t *p_this)
{
	itv_object_internals_t *p_priv = itv_object_internals(p_this);

	spin_lock(&p_priv->ref_spin);
	p_priv->refcount++;
	spin_unlock(&p_priv->ref_spin);

	return p_this;
}
EXPORT_SYMBOL(__itv_object_hold);

void __itv_object_set_destructor(itv_object_t *p_this, itv_destructor_t pf_destructor)
{
	itv_object_internals_t *p_priv = itv_object_internals(p_this);

	spin_lock(&p_priv->ref_spin);
	p_priv->pf_destructor = pf_destructor;
	spin_unlock(&p_priv->ref_spin);
}
EXPORT_SYMBOL(__itv_object_set_destructor);

static itv_object_t *FindObject(itv_object_t *p_this, int i_type, int i_mode)
{
	int i;
	
	itv_object_t *p_tmp;

	switch(i_mode & 0x000f) {
		case FIND_PARENT:
			p_tmp = p_this->parent;
			if(p_tmp) {
				if(itv_object_internals(p_tmp)->i_object_type == i_type) {
					itv_object_hold(p_tmp);
					return p_tmp;
				}
				else {
					return FindObject(p_tmp, i_type, i_mode);
				}
			}
			break;
		case FIND_CHILD:
			for(i = itv_object_internals(p_this)->i_children; i--; ) {
				p_tmp = itv_object_internals(p_this)->pp_children[i];
				if(itv_object_internals(p_tmp)->i_object_type == i_type) {
					itv_object_hold(p_tmp);
					return p_tmp;
				}
				else if(itv_object_internals(p_tmp)->i_children) {
					p_tmp = FindObject(p_tmp, i_type, i_mode);
					if(p_tmp)
						return p_tmp;
				}
			}
			break;
		case FIND_ANYWHERE:
			break;
	}

	return NULL;
}

static itv_object_t *FindObjectName(itv_object_t *p_this, const char *psz_name, int i_mode)
{
	int i;

	itv_object_t *p_tmp;

	switch(i_mode & 0x000f) {
		case FIND_PARENT:
			p_tmp = p_this->parent;
			if(p_tmp) {
				if(p_tmp->psz_object_name && !strcmp(p_tmp->psz_object_name, psz_name)) {
					itv_object_hold(p_tmp);
					return p_tmp;
				}
				else {
					return FindObjectName(p_tmp, psz_name, i_mode);
				}
			}
			break;
		case FIND_CHILD:
			for(i = itv_object_internals(p_this)->i_children; i--; ) {
				p_tmp = itv_object_internals(p_this)->pp_children[i];
				if(p_tmp->psz_object_name && !strcmp(p_tmp->psz_object_name, psz_name)) {
					itv_object_hold(p_tmp);
					return p_tmp;
				}
				else if(itv_object_internals(p_tmp)->i_children) {
					p_tmp = FindObjectName(p_tmp, psz_name, i_mode);
					if(p_tmp) return p_tmp;
				}
			}
			break;
		case FIND_ANYWHERE:
			break;
	}

	return NULL;
}

static int CountChildren(itv_object_t *p_this, int i_type)
{
	int i, i_count = 0;

	itv_object_t *p_tmp;

	for(i = 0; i < itv_object_internals(p_this)->i_children; i++) {
		p_tmp = itv_object_internals(p_this)->pp_children[i];

		if(itv_object_internals(p_tmp)->i_object_type == i_type) {
			i_count++;
		}
		i_count += CountChildren(p_tmp, i_type);
	}

	return i_count;
}

static itv_object_list_t *NewList(int i_count)
{
//20111213 koo : tcc88xx/tcc892x  kernel은 preempt kernel이기 때문에 mem alloc 시 GFP_KERNEL 대신 GFP_ATOMIC option 사용. 
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	itv_object_list_t *p_list = (itv_object_list_t *)kzalloc(sizeof(itv_object_list_t), GFP_ATOMIC);
#else
	itv_object_list_t *p_list = (itv_object_list_t *)kzalloc(sizeof(itv_object_list_t), GFP_KERNEL);
#endif

	if(p_list == NULL)
		return NULL;

	p_list->i_count = i_count;
	if(i_count == 0) {
		p_list->pp_object = NULL;
		return p_list;
	}

//20111213 koo : tcc88xx/tcc892x kernel은 preempt kernel이기 때문에 mem alloc 시 GFP_KERNEL 대신 GFP_ATOMIC option 사용. 
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
	p_list->pp_object = kzalloc(i_count * sizeof(itv_object_t *), GFP_ATOMIC);
#else
	p_list->pp_object = kzalloc(i_count * sizeof(itv_object_t *), GFP_KERNEL);
#endif

	if(p_list->pp_object == NULL) {
		p_list->i_count = 0;
		return p_list;
	}

	return p_list;
}

static void ListReplace(itv_object_list_t *p_list, itv_object_t *p_this, int i_index)
{
	if(p_list == NULL || i_index >= p_list->i_count)
		return;

	itv_object_hold(p_this);

	p_list->pp_object[i_index] = p_this;
	return;
}

static void ListChildren(itv_object_list_t *p_list, itv_object_t *p_this, int i_type)
{
	int i;

	itv_object_t *p_tmp;

	for(i = 0; i < itv_object_internals(p_this)->i_children; i++) {
		p_tmp = itv_object_internals(p_this)->pp_children[i];

		if(itv_object_internals(p_tmp)->i_object_type == i_type) {
			ListReplace(p_list, p_tmp, p_list->i_count++);
		}
		ListChildren(p_list, p_tmp, i_type);
	}
}

itv_object_t *__itv_object_find(itv_object_t *p_this, int i_type, int i_mode)
{
	itv_object_t *p_found;

	if((i_mode & 0x000f) == FIND_ANYWHERE)
		return itv_object_find(p_this->icore, i_type, (i_mode & ~0x000f) | FIND_CHILD);

	itv_lock(p_this->icore);
	p_found = FindObject(p_this, i_type, i_mode);
	itv_unlock(p_this->icore);

	return p_found;
}
EXPORT_SYMBOL(__itv_object_find);

itv_object_t *__itv_object_find_name(itv_object_t *p_this, const char *psz_name, int i_mode)
{
	itv_object_t *p_found;

	if(p_this->psz_object_name && !strcmp(p_this->psz_object_name, psz_name)) {
		itv_object_hold(p_this);
		return p_this;
	}

	itv_lock(p_this->icore);

	if((i_mode & 0x000f) == FIND_ANYWHERE) {
		itv_object_t *p_root = p_this;

		/* Find the root */
		while(p_root->parent != NULL && p_root != ITV_OBJECT(p_this->icore)) {
			p_root = p_root->parent;
		}

		p_found = FindObjectName(p_root, psz_name, (i_mode & ~0x000f) | FIND_CHILD);
		if(p_found == NULL && p_root != ITV_OBJECT(p_this->icore)) {
			p_found = FindObjectName(ITV_OBJECT(p_this->icore), psz_name, (i_mode & ~0x000f) | FIND_CHILD);
		}
	}
	else {
		p_found = FindObjectName(p_this, psz_name, i_mode);
	}

	itv_unlock(p_this->icore);

	return p_found;
}
EXPORT_SYMBOL(__itv_object_find_name);

itv_object_list_t *__itv_list_find(itv_object_t *p_this, int i_type, int i_mode)
{
	int i_count = 0;

	itv_object_list_t *p_list;

	switch(i_mode & 0x000f) {
		case FIND_ANYWHERE:
			return itv_list_find(p_this->icore, i_type, FIND_CHILD);
			break;
		case FIND_CHILD:
			itv_lock(p_this->icore);
			i_count = CountChildren(p_this, i_type);
			p_list = NewList(i_count);

			if(p_list->i_count != i_count) {
				itv_unlock(p_this->icore);
				p_list->i_count = 0;
				break;
			}

			p_list->i_count = 0;
			ListChildren(p_list, p_this, i_type);
			itv_unlock(p_this->icore);
			break;
		default:
			wprintk("unimplemented!\n");
			p_list = NewList(0);
			break;
	}

	return p_list;
}
EXPORT_SYMBOL(__itv_list_find);

itv_object_list_t *__itv_list_children(itv_object_t *p_this)
{
	int i;

	itv_object_list_t *p_list;
	itv_object_internals_t *priv = itv_object_internals(p_this);

	itv_lock(p_this->icore);
	p_list = NewList(priv->i_children);
	for(i = 0; i < p_list->i_count; i++) {
		itv_object_hold(priv->pp_children[i]);
		p_list->pp_object[i] = priv->pp_children[i];
	}
	itv_unlock(p_this->icore);

	return p_list;
}
EXPORT_SYMBOL(__itv_list_children);

void __itv_list_release(itv_object_list_t *p_list)
{
	int i_index;

	for(i_index = 0; i_index < p_list->i_count; i_index++) {
		itv_object_release(p_list->pp_object[i_index]);
	}
	kfree(p_list->pp_object);
	kfree(p_list);
}
EXPORT_SYMBOL(__itv_list_release);

void __itv_object_attach(itv_object_t *p_this, itv_object_t *p_parent)
{
	itv_object_internals_t *p_priv;

	if(!p_this) return;

	itv_object_hold(p_parent);
	itv_lock(p_this->icore);

	/* Attach the parent to its child */
	p_this->parent = p_parent;

	/* Attach the child to its parent */
	p_priv = itv_object_internals(p_parent);

	INSERT_ELEM(p_priv->pp_children, p_priv->i_children, p_priv->i_children, p_this);

	itv_unlock(p_this->icore);
}
EXPORT_SYMBOL(__itv_object_attach);

static void itv_object_detach_unlocked(itv_object_t *p_this)
{
	int i_index, i;

	itv_object_internals_t *p_priv;

	if(p_this->parent == NULL)
		return;

	p_priv = itv_object_internals(p_this->parent);
	p_this->parent = NULL;

	for(i_index = p_priv->i_children; i_index--; ) {
		if(p_priv->pp_children[i_index] == p_this) {
			p_priv->i_children--;
			for(i = i_index; i < p_priv->i_children; i++) {
				p_priv->pp_children[i] = p_priv->pp_children[i + 1];
			}
		}
	}

	if(p_priv->i_children) {
		itv_object_t **pp_children = (itv_object_t **)krealloc(p_priv->pp_children, 
				p_priv->i_children * sizeof(itv_object_t *), GFP_KERNEL);
		if(pp_children)
			p_priv->pp_children = pp_children;
	}
	else {
		kfree(p_priv->pp_children);
		p_priv->pp_children = NULL;
	}
}

void __itv_object_detach(itv_object_t *p_this)
{
	itv_object_t *parent;

	if(!p_this) return;

	itv_lock(p_this->icore);
	parent = p_this->parent;
	if(parent)
		itv_object_detach_unlocked(p_this);
	itv_unlock(p_this->icore);

	if(parent)
		itv_object_release(parent);
}
EXPORT_SYMBOL(__itv_object_detach);

static void itv_object_kill(itv_object_t *p_this)
{
	if(!p_this->b_die)
		p_this->b_die = true;
}

static void itv_object_destroy(itv_object_t *p_this)
{
	itv_object_internals_t *p_priv = itv_object_internals(p_this);

	itv_object_kill(p_this);

	if(p_priv->pf_destructor)
		p_priv->pf_destructor(p_this);

	if(p_this->psz_object_name)
		kfree(p_this->psz_object_name);

	kfree(p_priv);
}

itv_object_t *__itv_object_create(itv_object_t *p_this, size_t i_size, int i_type, const char *psz_type)
{
	itv_object_t *p_new = NULL;
	itv_object_internals_t *p_priv;

	if((p_priv = kzalloc(sizeof(*p_priv) + i_size, GFP_KERNEL)) == NULL) {
		eprintk("out of memory\n");
		return NULL;
	}
	p_new = (itv_object_t *)(p_priv + 1);

	p_priv->i_object_type = i_type;
	p_new->psz_object_type = psz_type;
	p_new->psz_object_name = NULL;

	p_new->b_error = false;
	p_new->b_die = false;

	if(p_this == NULL) {
		itv_core_t *p_self = (itv_core_t *)p_new;
		p_new->icore = p_self;
		spin_lock_init(&p_self->structure_lock);
		p_this = p_priv->next = p_priv->prev = p_new;
	}
	else
		p_new->icore = p_this->icore;

	spin_lock_init(&p_priv->ref_spin);
	p_priv->refcount = 1;
	p_priv->pf_destructor = NULL;
	p_new->parent = NULL;
	p_priv->pp_children = NULL;
	p_priv->i_children = 0;

	p_priv->next = p_this;
	itv_lock(p_new->icore);
	p_priv->prev = itv_object_internals(p_this)->prev;
	itv_object_internals(p_this)->prev = p_new;
	itv_object_internals(p_priv->prev)->next = p_new;
	itv_unlock(p_new->icore);

	return p_new;
}
EXPORT_SYMBOL(__itv_object_create);

void __itv_object_release(itv_object_t *p_this)
{
	bool b_should_destroy;
	unsigned int pre_ref;

	itv_object_internals_t *p_priv = itv_object_internals(p_this);
	itv_object_t *parent = NULL;

	spin_lock(&p_priv->ref_spin);

	pre_ref = p_priv->refcount;
	if(p_priv->refcount > 1) {
		p_priv->refcount--;
//		printk("[%s] refcount %d %d\n", p_this->psz_object_name, pre_ref, p_priv->refcount);
		spin_unlock(&p_priv->ref_spin);
		return;
	}
	spin_unlock(&p_priv->ref_spin);

	itv_lock(p_this->icore);

	spin_lock(&p_priv->ref_spin);
	b_should_destroy = --p_priv->refcount == 0;
//	printk("[%s] refcount %d %d\n", p_this->psz_object_name, pre_ref, p_priv->refcount);
	spin_unlock(&p_priv->ref_spin);

	if(b_should_destroy) {
		parent = p_this->parent;
		if(ITV_OBJECT(p_this->icore) == p_this) {
			itv_object_t *p_leaked = p_priv->next;
			while(p_leaked != p_this) {
				eprintk("leaking object (type:%s, name:%s)\n", 
						p_leaked->psz_object_type, p_leaked->psz_object_name);
				p_leaked = itv_object_internals(p_leaked)->next;
			}
		}

		itv_object_internals(p_priv->next)->prev = p_priv->prev;
		itv_object_internals(p_priv->prev)->next = p_priv->next;

		if(parent)
			itv_object_detach_unlocked(p_this);
	}
	itv_unlock(p_this->icore);

	if(b_should_destroy) {
		itv_object_destroy(p_this);
		if(parent)
			itv_object_release(parent);
	}
}
EXPORT_SYMBOL(__itv_object_release);
