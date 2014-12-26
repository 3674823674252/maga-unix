#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("49029TGJRKEGJ");
MODULE_DESCRIPTION("lexicographical spitter");

static unsigned long buffer_size = 4096;
module_param(buffer_size, ulong, (S_IRUSR| S_IRGRP | S_IROTH));
MODULE_PARAM_DESC(buffer_size, "Internal buffer size");

static int __init lex_init(void) {
  if (!buffer_size) {
    return -1;
  }
  printk(KERN_INFO "lex device has been registered, bufer size is %ld bytes\n", buffer_size);
  return 0
}

static void __exit lex_exit(void) {
  printk(KERN_INFO "lex device has been unregistered\n");
}

module_init(lex_init);
module_exit(lex_exit);