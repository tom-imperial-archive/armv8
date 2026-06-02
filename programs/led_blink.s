movz x0, #0x3f20, lsl #16
ldr w1, [x0, #8]
movz w2, #0x09
movz w3, #0x38
bic w1, w1, w3
orr w1, w1, w2
str w1, [x0, #8]

loop:

movz w1, #0x20, lsl #16
str w1, [x0, #0x1c]

movz w2, #0x0067, lsl #16
sleep:
subs w2, w2, #1
b.ne sleep

str w1, [x0, #0x28]

movz w2, #0x0067, lsl #16
sleep1:
subs w2, w2, #1
b.ne sleep1

b loop
