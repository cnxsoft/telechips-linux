#ifndef __ITV_ARRAY_H__
#define __ITV_ARRAY_H__

//20110706 koo : tcc88xx kernel은 preempt kernel이기 때문에 mem alloc 시 GFP_KERNEL 대신 GFP_ATOMIC option 사용. 
#if defined(CONFIG_ARCH_TCC88XX) || defined(CONFIG_ARCH_TCC892X)
#define INSERT_ELEM(p_ar, i_oldsize, i_pos, elem)									\
	do {																			\
		if(!i_oldsize) (p_ar) = NULL;												\
		(p_ar) = krealloc(p_ar, ((i_oldsize) + 1) * sizeof(*(p_ar)), GFP_ATOMIC);			\
		if((i_oldsize) - (i_pos)) {													\
			memmove((p_ar) + (i_pos) + 1, (p_ar) + (i_pos),							\
					((i_oldsize) - (i_pos)) * sizeof(*(p_ar)));						\
		}																			\
		(p_ar)[i_pos] = elem;														\
		(i_oldsize)++;																\
	} while(0)
#else
#define INSERT_ELEM(p_ar, i_oldsize, i_pos, elem)									\
	do {																			\
		if(!i_oldsize) (p_ar) = NULL;												\
		(p_ar) = krealloc(p_ar, ((i_oldsize) + 1) * sizeof(*(p_ar)), GFP_KERNEL);			\
		if((i_oldsize) - (i_pos)) {													\
			memmove((p_ar) + (i_pos) + 1, (p_ar) + (i_pos),							\
					((i_oldsize) - (i_pos)) * sizeof(*(p_ar)));						\
		}																			\
		(p_ar)[i_pos] = elem;														\
		(i_oldsize)++;																\
	} while(0)
#endif	//#if defined(CONFIG_ARCH_TCC88XX)

#endif	/* __ITV_ARRAY_H__ */
