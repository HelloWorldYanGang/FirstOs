#include "bootpack.h"

//缓存区初始化
void buffer_init(struct buffer *buffer, int size, unsigned char *buf)
{
	buffer->size = size;
	buffer->data = buf;
	buffer->free_size = size;
	buffer->flag = 0;
	buffer->next_write = 0;
	buffer->next_read = 0;
	return ;
}

//往缓存区写入一个数据,成功返回0，失败返回-1
int buffer_put(struct buffer *buffer, unsigned char data)
{
	if(buffer->free_size == 0)  //缓存区已满
	{
		buffer->flag |= FLAG_OVERFLOW;
		return -1;
	}
	buffer->data[buffer->next_write] = data;
	buffer->next_write++;
	if(buffer->next_write == buffer->size)
	{
		buffer->next_write = 0;
	}
	buffer->free_size--;
	return 0;
}

//往缓存区读取一个数据,成功返回0，失败返回-1
int buffer_get(struct buffer *buffer, unsigned char *ret_data)
{
	if(buffer->free_size == buffer->size)  //缓存区没有数据
	{
		return -1;
	}
	*ret_data = buffer->data[buffer->next_read];
	buffer->next_read++;
	if(buffer->next_read == buffer->size)
	{
		buffer->next_read = 0;
	}
	buffer->free_size++;
	return 0;
}

//计算剩余空间
int buffer_used_size(struct buffer *buffer)
{
	return buffer->size - buffer->free_size;
}