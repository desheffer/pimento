#include <assert.h>
#include <panic.h>
#include <scheduler.h>
#include <serial.h>
#include <unistd.h>

void sys_exit(int /*status*/)
{
    Scheduler::instance()->stopProcess();

    while (true) {
        asm volatile("wfi");
    }
}

ssize_t sys_read(int fd, void* buf, size_t count)
{
    char* cbuf = (char*) buf;
    ssize_t ret = 0;

    assert(fd == 0);

    while (count--) {
        *(cbuf++) = Serial::getc();
        ++ret;
    }

    return ret;
}

ssize_t sys_write(int fd, const void* buf, size_t count)
{
    const char* cbuf = (const char*) buf;
    ssize_t ret = 0;

    assert(fd == 1);

    while (count--) {
        Serial::putc(*(cbuf++));
        ++ret;
    }

    return ret;
}

void sys_invalid()
{
}

void* syscall_table[] = {
    (void*) sys_invalid, // 0
    (void*) sys_invalid, // 1
    (void*) sys_invalid, // 2
    (void*) sys_invalid, // 3
    (void*) sys_invalid, // 4
    (void*) sys_invalid, // 5
    (void*) sys_invalid, // 6
    (void*) sys_invalid, // 7
    (void*) sys_invalid, // 8
    (void*) sys_invalid, // 9
    (void*) sys_invalid, // 10
    (void*) sys_invalid, // 11
    (void*) sys_invalid, // 12
    (void*) sys_invalid, // 13
    (void*) sys_invalid, // 14
    (void*) sys_invalid, // 15
    (void*) sys_invalid, // 16
    (void*) sys_invalid, // 17
    (void*) sys_invalid, // 18
    (void*) sys_invalid, // 19
    (void*) sys_invalid, // 20
    (void*) sys_invalid, // 21
    (void*) sys_invalid, // 22
    (void*) sys_invalid, // 23
    (void*) sys_invalid, // 24
    (void*) sys_invalid, // 25
    (void*) sys_invalid, // 26
    (void*) sys_invalid, // 27
    (void*) sys_invalid, // 28
    (void*) sys_invalid, // 29
    (void*) sys_invalid, // 30
    (void*) sys_invalid, // 31
    (void*) sys_invalid, // 32
    (void*) sys_invalid, // 33
    (void*) sys_invalid, // 34
    (void*) sys_invalid, // 35
    (void*) sys_invalid, // 36
    (void*) sys_invalid, // 37
    (void*) sys_invalid, // 38
    (void*) sys_invalid, // 39
    (void*) sys_invalid, // 40
    (void*) sys_invalid, // 41
    (void*) sys_invalid, // 42
    (void*) sys_invalid, // 43
    (void*) sys_invalid, // 44
    (void*) sys_invalid, // 45
    (void*) sys_invalid, // 46
    (void*) sys_invalid, // 47
    (void*) sys_invalid, // 48
    (void*) sys_invalid, // 49
    (void*) sys_invalid, // 50
    (void*) sys_invalid, // 51
    (void*) sys_invalid, // 52
    (void*) sys_invalid, // 53
    (void*) sys_invalid, // 54
    (void*) sys_invalid, // 55
    (void*) sys_invalid, // 56
    (void*) sys_invalid, // 57
    (void*) sys_invalid, // 58
    (void*) sys_invalid, // 59
    (void*) sys_invalid, // 60
    (void*) sys_invalid, // 61
    (void*) sys_invalid, // 62
    (void*) sys_read, // 63
    (void*) sys_write, // 64
    (void*) sys_invalid, // 65
    (void*) sys_invalid, // 66
    (void*) sys_invalid, // 67
    (void*) sys_invalid, // 68
    (void*) sys_invalid, // 69
    (void*) sys_invalid, // 70
    (void*) sys_invalid, // 71
    (void*) sys_invalid, // 72
    (void*) sys_invalid, // 73
    (void*) sys_invalid, // 74
    (void*) sys_invalid, // 75
    (void*) sys_invalid, // 76
    (void*) sys_invalid, // 77
    (void*) sys_invalid, // 78
    (void*) sys_invalid, // 79
    (void*) sys_invalid, // 80
    (void*) sys_invalid, // 81
    (void*) sys_invalid, // 82
    (void*) sys_invalid, // 83
    (void*) sys_invalid, // 84
    (void*) sys_invalid, // 85
    (void*) sys_invalid, // 86
    (void*) sys_invalid, // 87
    (void*) sys_invalid, // 88
    (void*) sys_invalid, // 89
    (void*) sys_invalid, // 90
    (void*) sys_invalid, // 91
    (void*) sys_invalid, // 92
    (void*) sys_exit, // 93
    (void*) sys_invalid, // 94
    (void*) sys_invalid, // 95
    (void*) sys_invalid, // 96
    (void*) sys_invalid, // 97
    (void*) sys_invalid, // 98
    (void*) sys_invalid, // 99
    (void*) sys_invalid, // 100
    (void*) sys_invalid, // 101
    (void*) sys_invalid, // 102
    (void*) sys_invalid, // 103
    (void*) sys_invalid, // 104
    (void*) sys_invalid, // 105
    (void*) sys_invalid, // 106
    (void*) sys_invalid, // 107
    (void*) sys_invalid, // 108
    (void*) sys_invalid, // 109
    (void*) sys_invalid, // 110
    (void*) sys_invalid, // 111
    (void*) sys_invalid, // 112
    (void*) sys_invalid, // 113
    (void*) sys_invalid, // 114
    (void*) sys_invalid, // 115
    (void*) sys_invalid, // 116
    (void*) sys_invalid, // 117
    (void*) sys_invalid, // 118
    (void*) sys_invalid, // 119
    (void*) sys_invalid, // 120
    (void*) sys_invalid, // 121
    (void*) sys_invalid, // 122
    (void*) sys_invalid, // 123
    (void*) sys_invalid, // 124
    (void*) sys_invalid, // 125
    (void*) sys_invalid, // 126
    (void*) sys_invalid, // 127
    (void*) sys_invalid, // 128
    (void*) sys_invalid, // 129
    (void*) sys_invalid, // 130
    (void*) sys_invalid, // 131
    (void*) sys_invalid, // 132
    (void*) sys_invalid, // 133
    (void*) sys_invalid, // 134
    (void*) sys_invalid, // 135
    (void*) sys_invalid, // 136
    (void*) sys_invalid, // 137
    (void*) sys_invalid, // 138
    (void*) sys_invalid, // 139
    (void*) sys_invalid, // 140
    (void*) sys_invalid, // 141
    (void*) sys_invalid, // 142
    (void*) sys_invalid, // 143
    (void*) sys_invalid, // 144
    (void*) sys_invalid, // 145
    (void*) sys_invalid, // 146
    (void*) sys_invalid, // 147
    (void*) sys_invalid, // 148
    (void*) sys_invalid, // 149
    (void*) sys_invalid, // 150
    (void*) sys_invalid, // 151
    (void*) sys_invalid, // 152
    (void*) sys_invalid, // 153
    (void*) sys_invalid, // 154
    (void*) sys_invalid, // 155
    (void*) sys_invalid, // 156
    (void*) sys_invalid, // 157
    (void*) sys_invalid, // 158
    (void*) sys_invalid, // 159
    (void*) sys_invalid, // 160
    (void*) sys_invalid, // 161
    (void*) sys_invalid, // 162
    (void*) sys_invalid, // 163
    (void*) sys_invalid, // 164
    (void*) sys_invalid, // 165
    (void*) sys_invalid, // 166
    (void*) sys_invalid, // 167
    (void*) sys_invalid, // 168
    (void*) sys_invalid, // 169
    (void*) sys_invalid, // 170
    (void*) sys_invalid, // 171
    (void*) sys_invalid, // 172
    (void*) sys_invalid, // 173
    (void*) sys_invalid, // 174
    (void*) sys_invalid, // 175
    (void*) sys_invalid, // 176
    (void*) sys_invalid, // 177
    (void*) sys_invalid, // 178
    (void*) sys_invalid, // 179
    (void*) sys_invalid, // 180
    (void*) sys_invalid, // 181
    (void*) sys_invalid, // 182
    (void*) sys_invalid, // 183
    (void*) sys_invalid, // 184
    (void*) sys_invalid, // 185
    (void*) sys_invalid, // 186
    (void*) sys_invalid, // 187
    (void*) sys_invalid, // 188
    (void*) sys_invalid, // 189
    (void*) sys_invalid, // 190
    (void*) sys_invalid, // 191
    (void*) sys_invalid, // 192
    (void*) sys_invalid, // 193
    (void*) sys_invalid, // 194
    (void*) sys_invalid, // 195
    (void*) sys_invalid, // 196
    (void*) sys_invalid, // 197
    (void*) sys_invalid, // 198
    (void*) sys_invalid, // 199
    (void*) sys_invalid, // 200
    (void*) sys_invalid, // 201
    (void*) sys_invalid, // 202
    (void*) sys_invalid, // 203
    (void*) sys_invalid, // 204
    (void*) sys_invalid, // 205
    (void*) sys_invalid, // 206
    (void*) sys_invalid, // 207
    (void*) sys_invalid, // 208
    (void*) sys_invalid, // 209
    (void*) sys_invalid, // 210
    (void*) sys_invalid, // 211
    (void*) sys_invalid, // 212
    (void*) sys_invalid, // 213
    (void*) sys_invalid, // 214
    (void*) sys_invalid, // 215
    (void*) sys_invalid, // 216
    (void*) sys_invalid, // 217
    (void*) sys_invalid, // 218
    (void*) sys_invalid, // 219
    (void*) sys_invalid, // 220
    (void*) sys_invalid, // 221
    (void*) sys_invalid, // 222
    (void*) sys_invalid, // 223
    (void*) sys_invalid, // 224
    (void*) sys_invalid, // 225
    (void*) sys_invalid, // 226
    (void*) sys_invalid, // 227
    (void*) sys_invalid, // 228
    (void*) sys_invalid, // 229
    (void*) sys_invalid, // 230
    (void*) sys_invalid, // 231
    (void*) sys_invalid, // 232
    (void*) sys_invalid, // 233
    (void*) sys_invalid, // 234
    (void*) sys_invalid, // 235
    (void*) sys_invalid, // 236
    (void*) sys_invalid, // 237
    (void*) sys_invalid, // 238
    (void*) sys_invalid, // 239
    (void*) sys_invalid, // 240
    (void*) sys_invalid, // 241
    (void*) sys_invalid, // 242
    (void*) sys_invalid, // 243
    (void*) sys_invalid, // 244
    (void*) sys_invalid, // 245
    (void*) sys_invalid, // 246
    (void*) sys_invalid, // 247
    (void*) sys_invalid, // 248
    (void*) sys_invalid, // 249
    (void*) sys_invalid, // 250
    (void*) sys_invalid, // 251
    (void*) sys_invalid, // 252
    (void*) sys_invalid, // 253
    (void*) sys_invalid, // 254
    (void*) sys_invalid, // 255
    (void*) sys_invalid, // 256
    (void*) sys_invalid, // 257
    (void*) sys_invalid, // 258
    (void*) sys_invalid, // 259
    (void*) sys_invalid, // 260
    (void*) sys_invalid, // 261
    (void*) sys_invalid, // 262
    (void*) sys_invalid, // 263
    (void*) sys_invalid, // 264
    (void*) sys_invalid, // 265
    (void*) sys_invalid, // 266
    (void*) sys_invalid, // 267
    (void*) sys_invalid, // 268
    (void*) sys_invalid, // 269
    (void*) sys_invalid, // 270
    (void*) sys_invalid, // 271
    (void*) sys_invalid, // 272
    (void*) sys_invalid, // 273
    (void*) sys_invalid, // 274
    (void*) sys_invalid, // 275
    (void*) sys_invalid, // 276
    (void*) sys_invalid, // 277
    (void*) sys_invalid, // 278
    (void*) sys_invalid, // 279
    (void*) sys_invalid, // 280
    (void*) sys_invalid, // 281
    (void*) sys_invalid, // 282
    (void*) sys_invalid, // 283
    (void*) sys_invalid, // 284
    (void*) sys_invalid, // 285
    (void*) sys_invalid, // 286
    (void*) sys_invalid, // 287
    (void*) sys_invalid, // 288
    (void*) sys_invalid, // 289
    (void*) sys_invalid, // 290
    (void*) sys_invalid, // 291
    (void*) sys_invalid, // 292
};
