void free(void *p, heap_t *heap){
	.....
	header_t *header=(header_t*)((u32int)p-sizeof(header_t));
	footer_t *footer=(footer_t*)((u32int)header+header->size-sizeof(footer_t));
	
	//Sanity checks
	SKY_ASSERT(header->magic == HEAP_MAGIC, "header->magic == HEAP_MAGIC");//heap currption checksum비교
	SKY_ASSERT(footer->magic == HEAP_MAGIC, "footerer->magic == HEAP_MAGIC");//buffer overrun checksum비교 
}
