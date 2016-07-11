#include <bitset>
#include <iostream>
using namespace std;

#define ADDR (*(volatile long *) addr)

static __inline__ int constant_test_bit(int nr, const volatile void * addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

static __inline__ int variable_test_bit(int nr, volatile const void * addr)
{
	int oldbit;

	__asm__ __volatile__(
			"btl %2,%1\n\tsbbl %0,%0"
			:"=r" (oldbit)
			:"m" (ADDR),"dIr" (nr));
	return oldbit;
}

#define test_bit(nr,addr) \
	(__builtin_constant_p(nr) ? \
	 constant_test_bit((nr),(addr)) : \
	 variable_test_bit((nr),(addr)))

	static inline long
__find_first_bit(const unsigned long * addr, unsigned long size)
{
	long d0, d1;
	long res;

	/*
	 * We must test the size in words, not in bits, because
	 * otherwise incoming sizes in the range -63..-1 will not run
	 * any scasq instructions, and then the flags used by the jz
	 * instruction will have whatever random value was in place
	 * before.  Nobody should call us like that, but
	 * find_next_bit() does when offset and size are at the same
	 * word and it fails to find a one itself.
	 */
	size += 63;
	size >>= 6;
	if (!size)
		return 0;
	asm volatile(
			"   repe; scasq\n"
			"   jz 1f\n"
			"   subq $8,%%rdi\n"
			"   bsfq (%%rdi),%%rax\n"
			"1: subq %[addr],%%rdi\n"
			"   shlq $3,%%rdi\n"
			"   addq %%rdi,%%rax"
			:"=a" (res), "=&c" (d0), "=&D" (d1)
			:"0" (0ULL), "1" (size), "2" (addr),
			[addr] "r" (addr) : "memory");
	return res;
}

/**
 * find_first_bit - find the first set bit in a memory region
 * @addr: The address to start the search at
 * @size: The maximum size to search
 *
 * Returns the bit-number of the first set bit, not the number of the byte
 * containing a bit.
 */
long find_first_bit(const unsigned long * addr, unsigned long size)
{
	return __find_first_bit(addr,size);
}

/**
 * find_next_bit - find the first set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The maximum size to search
 */
long find_next_bit(const unsigned long * addr, long size, long offset)
{
	const unsigned long * p = addr + (offset >> 6);
	unsigned long set = 0, bit = offset & 63, res;

	if (bit) {
		/*
		 * Look for nonzero in the first 64 bits:
		 */
		asm("bsfq %1,%0\n\t"
				"cmoveq %2,%0\n\t"
				: "=r" (set)
				: "r" (*p >> bit), "r" (64L));
		if (set < (64 - bit))
			return set + offset;
		set = 64 - bit;
		p++;
	}
	/*
	 * No set bit yet, search remaining full words for a bit
	 */
	res = __find_first_bit (p, size - 64 * (p - addr));
	return (offset + set + res);
}

void set_bit(int nr, volatile void * addr)
{
	__asm__ volatile(
			"btsl %1,%0"
			:"+m" (ADDR)
			:"dIr" (nr) : "memory");
}



void clear_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__(
			"btrl %1,%0"
			:"+m" (ADDR)
			:"dIr" (nr));
}


int main()
{
	unsigned long bitmap[2];
	bitmap[0] = 0xff0000;
	bitmap[1] = 0xff0000;
	int rc = 0;
	while( rc < 128){
		rc = find_next_bit(bitmap, 128, rc+1);
		cout << "rc: " << rc << endl;
	}
	clear_bit(16, bitmap);
	set_bit(13, bitmap);
	rc = 0;
	while( rc < 128){
		rc = find_next_bit(bitmap, 128, rc+1);
		cout << "rc: " << rc << endl;
	}

}
