/*
 * FB driver for the TonTec 3.5 LCD Controller
 *
 * Copyright (c) 2014 Jon Corss
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>

#include "fbtft.h"

#define DRVNAME		"fb_tontec35"
#define WIDTH		320
#define HEIGHT		480

static int default_init_sequence[] = {
	-1, 0x11,
	-2, 100,
	-1, 0x13,
	-2, 100,
	-1, 0x3A, 0x55,
	-1, 0xC2, 0x44,
	-1, 0xC5, 0x00, 0x00, 0x00, 0x00,
	-1, 0xE0, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08,
			  0x48, 0x98, 0x37, 0x0A, 0x13, 0x04,
			  0x11, 0x0D, 0x00,
	-1, 0xE1, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05,
			  0x47, 0x75, 0x37, 0x06, 0x10, 0x03,
			  0x24, 0x20, 0x00,
	-1, 0xE2, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05,
			  0x47, 0x75, 0x37, 0x06, 0x10, 0x03,
			  0x24, 0x20, 0x00,
	-1, 0x2B, 0x00, 0x00, 0x01, 0x3F,
	-1, 0x2A, 0x00, 0x00, 0x01, 0xDf,
	-1, 0x11,
	-2, 100,
	-1, 0x29,
	-3
};

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	fbtft_par_dbg(DEBUG_SET_ADDR_WIN, par,
		"%s(xs=%d, ys=%d, xe=%d, ye=%d)\n", __func__, xs, ys, xe, ye);

	/* column address */
	write_reg(par, 0x2a, xs >> 8, xs & 0xff, xe >> 8, xe & 0xff);

	/* row adress */
	write_reg(par, 0x2b, ys >> 8, ys & 0xff, ye >> 8, ye & 0xff);

	/* memory write */
	write_reg(par, 0x2c);
}

#define HFLIP		0x01	/* horizontal flip - flipped display */
#define VFLIP		0x02	/* vertical flip - flipped display */
#define ROWxCOL		0x20	/* page/column order - reverse mode */
#define COLADDRORD	0x40	/* column address order - right to left */
#define PAGEADDRORD	0x80	/* page address order - bottom to top */
static int set_var(struct fbtft_par *par)
{
	fbtft_par_dbg(DEBUG_INIT_DISPLAY, par, "%s()\n", __func__);
	switch (par->info->var.rotate) {
	case 270:
		write_reg(par, 0x36, PAGEADDRORD | COLADDRORD | ROWxCOL
					| HFLIP | VFLIP | (par->bgr << 3));
		break;
	case 180:
		write_reg(par, 0x36, PAGEADDRORD | VFLIP | (par->bgr << 3));
		break;
	case 90:
		write_reg(par, 0x36, ROWxCOL | (par->bgr << 3));
		break;
	default:
		write_reg(par, 0x36, COLADDRORD | HFLIP | (par->bgr << 3));
		break;
	}
	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.init_sequence = default_init_sequence,
	.fbtftops = {
		.set_addr_win = set_addr_win,
		.set_var = set_var,
	},
};
FBTFT_REGISTER_DRIVER(DRVNAME, "itontec,tontec35", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:tontec35");
MODULE_ALIAS("platform:tontec35");

MODULE_DESCRIPTION("FB driver for the TonTec 3.5 LCD Controller");
MODULE_AUTHOR("Jon Cross");
MODULE_LICENSE("GPL");
