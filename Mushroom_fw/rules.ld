/* Stack rules inclusion.*/
INCLUDE ../rules_stacks.ld

/* Code rules inclusion.*/
INCLUDE ../rules_code.ld

/* Data rules inclusion.*/
INCLUDE ../rules_data.ld

/***************************************
 *  Here is place to read and write to *
 ***************************************/
SECTIONS {
    MyFlash1 0x0800E400 :
    {
        *(MyFlash)
    } > flash0
}
