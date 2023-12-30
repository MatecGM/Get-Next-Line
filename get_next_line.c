/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbico <mbico@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 14:34:13 by mbico             #+#    #+#             */
/*   Updated: 2023/12/14 11:35:35 by mbico            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*clean_buffer(char *buffer, int *error)
{
	char	*tmp;
	int		len;
	int		i;
	int		bufferlen;

	i = 0;
	len = ft_strbanlen(buffer, '\n');
	bufferlen = ft_strbanlen(buffer, 0);
	if (len == bufferlen)
		return (ft_freeall(buffer, NULL));
	tmp = buffer;
	buffer = ft_calloc((bufferlen - len + 1), sizeof(char));
	if (!buffer)
	{
		*error = -1;
		return (NULL);
	}
	while (len < bufferlen)
		buffer[i++] = tmp[len++];
	buffer[i] = 0;
	free(tmp);
	return (buffer);
}

char	*find_endline(char *line, char **buffer, int fd, int *error)
{
	char	*tmp;
	ssize_t	readed;

	while (line && line[ft_strbanlen(line, 0) - 1] != '\n')
	{
		if (!*buffer)
		{
			*buffer = ft_calloc(BUFFER_SIZE + 1, sizeof(char));
			if (!*buffer)
				return (NULL);
		}
		readed = read(fd, *buffer, BUFFER_SIZE);
		tmp = ft_strdupn(*buffer);
		line = ft_strjoin(line, tmp);
		*buffer = clean_buffer(*buffer, error);
		if (*error == -1 || !line)
			return (NULL);
		if (readed < BUFFER_SIZE)
			return (line);
	}
	return (line);
}

char	*ft_bufferempty(char **buffer, char *line, int fd, int *error)
{
	ssize_t		readed;

	*buffer = ft_calloc((BUFFER_SIZE + 1), sizeof(char));
	if (!*buffer)
		return (NULL);
	readed = read(fd, *buffer, BUFFER_SIZE);
	if (readed <= 0)
	{
		free(*buffer);
		*buffer = NULL;
		return (NULL);
	}
	line = ft_strdupn(*buffer);
	*buffer = clean_buffer(*buffer, error);
	if (*error == -1 || !line)
		return (NULL);
	if (readed == BUFFER_SIZE)
	{
		line = find_endline(line, buffer, fd, error);
		if (*error == -1)
			return (NULL);
	}
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buffer;
	char		*line;
	int			error;

	if (fd < 0)
		return (NULL);
	line = NULL;
	error = 0;
	if (buffer)
	{
		line = ft_strdupn(buffer);
		buffer = clean_buffer(buffer, &error);
		if (error == -1 || !line)
			return (ft_freeall(buffer, line));
		line = find_endline(line, &buffer, fd, &error);
		if (error == -1)
			return (ft_freeall(buffer, line));
	}
	else
	{
		line = ft_bufferempty(&buffer, line, fd, &error);
		if (error == -1)
			return (ft_freeall(buffer, line));
	}
	return (line);
}

#include <stdio.h>
#include <fcntl.h> 

int	main(void)
{
	int fd;
	char *line;

	fd = open("salut", O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("|%s", line);
		free(line);
		line = get_next_line(fd);
	}
}