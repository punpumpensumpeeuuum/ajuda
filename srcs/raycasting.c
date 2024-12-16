/* ************************************************************************** */
/*	                                                                        */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jomendes <jomendes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 12:38:37 by jomendes          #+#    #+#             */
/*   Updated: 2024/12/10 17:22:51 by jomendes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"
#include <float.h>

void	dda_style(t_vc *vc)
{
	int		x;
	double	ray_camera;

	x = 0;
	vc->player.direction_x = cos(degree_to_radian(vc->player.facing));
	vc->player.direction_x = sin(degree_to_radian(vc->player.facing));
	while (x < X_SCREEN)
	{
		vc->player.plane_x = -vc->player.direction_y * 0.66;
		vc->player.plane_y = vc->player.direction_x * 0.66;
		ray_camera = (2 * x) / (double)X_SCREEN - 1;
		vc->ray.id = x;
		vc->ray.pos_x = vc->player.pos_x;
		vc->ray.pos_y = vc->player.pos_y;
		vc->ray.direction_x = vc->player.direction_x + vc->player.plane_x * ray_camera;
		vc->ray.direction_y = vc->player.direction_y + vc->player.plane_y * ray_camera;
		if (vc->ray.direction_x == 0)
			vc->ray.delta_dist_x = 1e30;
		else
			vc->ray.delta_dist_x = fabs(1 / vc->ray.direction_x);
		if (vc->ray.direction_y == 0)
			vc->ray.delta_dist_y = 1e30;
		else
			vc->ray.delta_dist_y = fabs(1 / vc->ray.direction_y);
		// printf("\n\n");
		// printf("vc->ray.pos_x = %d\n", vc->ray.pos_x);
		// printf("vc->ray.pos_y = %d\n", vc->ray.pos_y);
		// printf("vc->ray.direction_x = %f\n", vc->ray.direction_x);
		// printf("vc->ray.direction_y = %f\n", vc->ray.direction_y);
		// printf("\n\n");
		dda_step_calc(vc);
		printf("2\n");
		dda_real_distance_calc(vc);
		printf("3\n");
		dda_wall_height(&vc->ray);
		printf("4\n");
		dda_side_selector(vc, &vc->ray, &vc->player, &vc->map_info);
		printf("5\n");
		x++;
	}
	printf("pointer = %p\n", vc->data.img_ptr);
	printf("size y = %d, size x = %d\n", vc->data.img_size_y, vc->data.img_size_x);
	vc->data = *vc->map_info.no_texture;
	mlx_put_image_to_window(vc->mlx.mlx, vc->mlx.window, vc->data.img_ptr, vc->data.img_size_y, vc->data.img_size_x);
}

void	dda_step_calc(t_vc *vc)
{
	if (vc->ray.direction_x < 0)
	{
		vc->ray.step_x = -1;
		vc->ray.distance_x = (vc->player.pos_x - vc->ray.pos_x)
			* vc->ray.delta_dist_x;
	}
	else
	{
		vc->ray.step_x = 1;
		vc->ray.distance_x = (vc->ray.pos_x + 1.0 - vc->player.pos_x)
			* vc->ray.delta_dist_x;
	}
	if (vc->ray.direction_y < 0)
	{
		vc->ray.step_y = -1;
		vc->ray.distance_y = (vc->player.pos_y - vc->ray.pos_y) * vc->ray.delta_dist_y;
	}
	else
	{
		vc->ray.step_y = 1;
		vc->ray.distance_y = (vc->ray.pos_y + 1.0 - vc->player.pos_y)
			* vc->ray.delta_dist_y;
	}
}

void	dda_real_distance_calc(t_vc *vc)
{
	int	hit;

	hit = 0;
    while (hit == 0)
    {
		if (vc->ray.distance_x < vc->ray.distance_y)
		{
			vc->ray.distance_x += vc->ray.delta_dist_x;
			vc->ray.pos_x += vc->ray.step_x;
			vc->ray.side = 0;
		}
		else
		{
			vc->ray.distance_y += vc->ray.delta_dist_y;
			vc->ray.pos_y += vc->ray.step_y;
			vc->ray.side = 1;
		}
		if (vc->map.matrix[vc->ray.pos_y][vc->ray.pos_x] == '1')
			hit = 1;

	}
	if (vc->ray.side == 0)
		vc->ray.real_size = (vc->ray.distance_x - vc->ray.delta_dist_x);
	else
		vc->ray.real_size = (vc->ray.distance_y - vc->ray.delta_dist_y);
}

void	dda_wall_height(t_ray *ray)
{
	ray->line_height = (int)(Y_SCREEN / ray->real_size);
	ray->wall_start = -ray->line_height / 2 + Y_SCREEN / 2;
	if (ray->wall_start < 0 || ray->line_height < 0)
		ray->wall_start = 0;
	ray->wall_end = ray->line_height / 2 + Y_SCREEN / 2;
	if (ray->wall_end >= Y_SCREEN || ray->line_height < 0)
		ray->wall_end = Y_SCREEN - 1;
}

void	dda_side_selector(t_vc *vc, t_ray *ray, t_player *player, t_map_info *info)
{
	double	wall_x;

	// printf("Address: %s\n", vc->map_info.no_texture->addr);
	// printf("Address: %s\n", vc->map_info.so_texture->addr);
	// printf("Address: %s\n", vc->map_info.we_texture->addr);
	// printf("Address: %s\n", vc->map_info.so_texture->addr);
	if (ray->side == 0)
		wall_x = player->pos_y + ray->real_size * ray->direction_y;
	else
		wall_x = player->pos_x + ray->real_size * ray->direction_x;
	wall_x -= floor(wall_x);
	ray->x_texture = (int)(wall_x * 64);
	if (ray->side == 0 && ray->direction_x > 0)
		ray->x_texture = 64 - ray->x_texture - 1;
	if (ray->side == 1 && ray->direction_y < 0)
		ray->x_texture = 64 - ray->x_texture - 1;
	if (ray->side == 0 && ray->direction_x > 0)
		draw_walls(vc, ray, info->ea_texture);
	else if (ray->side == 0 && ray->direction_x < 0)
		draw_walls(vc, ray, info->we_texture);
	else if (ray->side == 1 && ray->direction_y > 0)
		draw_walls(vc, ray, info->so_texture);
	else if (ray->side == 1 && ray->direction_y < 0)
		draw_walls(vc, ray, info->no_texture);
}

void	my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	if (x < 0 || x >= data->img_size_x || y < 0 || y >= data->img_size_y)
        return;
	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int *)dst = color;
}

int	get_raycolor(int tex_x, int tex_y, t_data *data)
{
	int	color;
	int	offset;

	offset = tex_y * data->line_length + tex_x * (data->bits_per_pixel / 8);
	color = *(int *)(data->addr + offset);
	return (color);
}

void	draw_walls(t_vc *vc, t_ray *ray, t_data *texture)
{
	double	step;
	double	texture_pos;
	int		texture_y;
	int		color;
	int		y;

	step = 1.0 * texture->img_size_y / ray->line_height;
	texture_pos = (ray->wall_start - Y_SCREEN / 2
			+ ray->line_height / 2) * step;
	y = ray->wall_start;
	while (y < ray->wall_end)
	{
		texture_y = (int)texture_pos & (texture->img_size_y - 1);
		color = get_raycolor(ray->x_texture, texture_y, texture);
		// printf("ray id = %d\n", ray->id);
		// printf("y = %d\n", y);
		my_mlx_pixel_put(texture, ray->id, y, color);
		texture_pos += step;
		y++;
	}
	draw_floor_ceiling(vc, ray);
}

void	draw_floor_ceiling(t_vc *vc, t_ray *ray)
{
	int	c_color;
	int	f_color;
	int	y;

	//c_color = create_rgb(vc->map_info.ceiling_color);
	//f_color = create_rgb(vc->map_info.ceiling_color);
	c_color = 0xFFFFFF;
	f_color = 0x000000;
	y = 0;
	while (y < ray->wall_start)
	{
		my_mlx_pixel_put(&vc->data, ray->id, y, c_color);
		y++;
	}
	y = ray->wall_end + 1;
	while (y < Y_SCREEN)
	{
		my_mlx_pixel_put(&vc->data, ray->id, y, c_color);
		y++;
	}
}