/*
 * include/linux/journal-head.h
 *
 * buffer_head fields for JBD
 *
 * 27 May 2001 Andrew Morton <akpm@digeo.com>
 *	Created - pulled out of fs.h
 */

#ifndef JOURNAL_HEAD_H_INCLUDED
#define JOURNAL_HEAD_H_INCLUDED

typedef unsigned int		tid_t;		/* Unique transaction ID */
typedef struct transaction_s	transaction_t;	/* Compound transaction type */
struct buffer_head;

/**
 * JBD用此描述符管理磁盘块
 * 与buffer_head对应
 */
struct journal_head {
	/*
	 * Points back to our buffer_head. [jbd_lock_bh_journal_head()]
	 */
	/* 指向所管理的磁盘块 */
	struct buffer_head *b_bh;

	/*
	 * Reference count - see description in journal.c
	 * [jbd_lock_bh_journal_head()]
	 */
	/* 引用计数 */
	int b_jcount;

	/*
	 * Journalling list for this buffer [jbd_lock_bh_state()]
	 */
	/**
	 * 当前缓冲区位于日志的哪个链表中。
	 */
	unsigned b_jlist;

	/*
	 * Copy of the buffer data frozen for writing to the log.
	 * [jbd_lock_bh_state()]
	 */
	/**
	 * 如果需要转义，就将buffer_head中的数据复制到这里
	 * 将备份的数据写到日志中
	 */
	char *b_frozen_data;

	/*
	 * Pointer to a saved copy of the buffer containing no uncommitted
	 * deallocation references, so that allocations can avoid overwriting
	 * uncommitted deletes. [jbd_lock_bh_state()]
	 */
	/**
	 * 位图磁盘缓冲区
	 * 这里将其复制一份写到日志中
	 */
	char *b_committed_data;

	/*
	 * Pointer to the compound transaction which owns this buffer's
	 * metadata: either the running transaction or the committing
	 * transaction (if there is one).  Only applies to buffers on a
	 * transaction's data or metadata journaling list.
	 * [j_list_lock] [jbd_lock_bh_state()]
	 */
	/* 指向所属事务 */
	transaction_t *b_transaction;

	/*
	 * Pointer to the running compound transaction which is currently
	 * modifying the buffer's metadata, if there was already a transaction
	 * committing it when the new transaction touched it.
	 * [t_list_lock] [jbd_lock_bh_state()]
	 */
	/**
	 * 正在修改缓冲区的事务。
	 * 此时已经有一个事务在提交缓冲区，而新事务在访问它
	 */
	transaction_t *b_next_transaction;

	/*
	 * Doubly-linked list of buffers on a transaction's data, metadata or
	 * forget queue. [t_list_lock] [jbd_lock_bh_state()]
	 */
	/**
	 * 通过这两个字段将缓冲区链接到事务的链表中
	 */
	struct journal_head *b_tnext, *b_tprev;

	/*
	 * Pointer to the compound transaction against which this buffer
	 * is checkpointed.  Only dirty buffers can be checkpointed.
	 * [j_list_lock]
	 */
	/**
	 * 正在checkpoint本缓冲区的事务
	 */
	transaction_t *b_cp_transaction;

	/*
	 * Doubly-linked list of buffers still remaining to be flushed
	 * before an old transaction can be checkpointed.
	 * [j_list_lock]
	 */
	/**
	 * 通过这两个字段，将其链接到事务的checkpoint链表中
	 */
	struct journal_head *b_cpnext, *b_cpprev;
};

#endif		/* JOURNAL_HEAD_H_INCLUDED */
