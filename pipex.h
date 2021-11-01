/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalvarad <jalvarad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/14 09:43:32 by jalvarad          #+#    #+#             */
/*   Updated: 2021/11/01 14:05:48 by jalvarad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H
# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/wait.h>
# ifndef READ_END
#  define READ_END 0
# endif
# ifndef WRITE_END
#  define WRITE_END 1
# endif

typedef struct s_pipe_var
{
	int		fd1;
	int		**fd2;
	int		status;
	pid_t	pid;
	char	*path;
	int		l_p;/// last_pipe
	int		n_p;/// next_pipe
}t_pipe_var;

typedef struct s_cmds
{
	char			**content;
	struct s_cmds	*next;
}	t_cmds;

void	*ft_memcpy(void *dst, const void *src, size_t n);
size_t	ft_strlen(const char *s);
char	*ft_strnstr(const char *haystack, const char *needle, size_t len);
char	*ft_strjoin(char const *s1, char const *s2);
char	**ft_split(const char *s, char c);
void	del_split(char **split);
void	ft_putstr_fd(char *s, int fd);
void	rev_and_exe(char *path, char **argv, char **envp);
void	error_of_cmd(char **cmd);
char	*search_path(char *argv, char **envp);
//void	kamikaze_son1(t_pipe_var info, char *argv, char **envp, char *infile);
char	**ft_mod_split(const char *s, char c);
void	ft_lstadd_back(t_cmds **lst, t_cmds *new);
t_cmds	*ft_lstnew(char **content);
int	ft_lstsize(t_cmds *lst);
#endif