/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fractol.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgallet <tgallet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 16:24:42 by tgallet           #+#    #+#             */
/*   Updated: 2025/02/10 12:40:43 by tgallet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRACTOL_H
# define FRACTOL_H
# include "minilibx-linux/mlx.h"
# include "libft/libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <unistd.h>

typedef struct s_myimg {
	void			*img;
	int				bits_per_pixel;
	int				line_length;
	int				width;
	int				height;
	char			*addr;
	double			inv_width;
	double			inv_height;
	double			ratio;
	unsigned int	*pixel;
	int				endian;
}	t_myimg;

typedef struct s_params
{
	double			tx;
	double			ty;
	double			cx;
	double			cy;
	double			scale;
	unsigned int	(*palette)(int iteration, int max_it);
}	t_params;

typedef struct s_mcolor
{
	unsigned char	blue;
	unsigned char	green;
	unsigned char	red;
	unsigned char	alpha;
}	t_mcolor;

typedef struct s_pixel_info
{
	double			x;
	double			y;
	double			cx;
	double			cy;
	double			scale;
	int				iter_max;
	unsigned int	(*palette)(int iteration, int max_it);
}	t_pixel_info;

typedef struct s_loopdata
{
	t_myimg			imgd1;
	t_myimg			imgd2;
	t_myimg			*current;
	t_myimg			*tmp;
	void			*mlx;
	void			*win;
	unsigned int	(*plot)(t_pixel_info pi);
	struct s_params	par;
	unsigned int	change;
}	t_loopdata;

int				ft_printf(const char *format, ...);
void			set_pixel_int(t_myimg *data, int x, int y, unsigned int color);
void			set_pixel_color(t_myimg *data, int x, int y, t_mcolor color);
void			swap_img(t_loopdata *ld);
void			zerod_loopdata(t_loopdata *ld);
void			free_loopdata(t_loopdata **ld);
t_loopdata		*init_loopdata(int width, int height, double cx, double cy);
t_mcolor		hs_to_rgb(double h, double s);
int				ondestroy(t_loopdata **ld);
int				onmouse(int button, int x, int y, t_loopdata *ld);
int				onkey(int keycode, t_loopdata **ld);
void			loop_pixels(t_myimg *img, t_params par,
					unsigned int (*pix_fun)(t_pixel_info pi));
unsigned int	mandelbrot(t_pixel_info pi);
unsigned int	julia(t_pixel_info pi);
unsigned int	reddish(int iteration, int max_iteration);
unsigned int	no_palette(int iteration, int max_iteration);
void			set_next_pixel(t_myimg *data, int x, int y, unsigned int color);
int				loop_hook(t_loopdata *ld);
double			ft_atod(char *s);
void			color_shift_img(t_myimg *img, unsigned int change);
t_mcolor		get_pixel(t_myimg *data, int x, int y);
unsigned int	burning_ship(t_pixel_info pi);

#endif
