/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalvarad <jalvarad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/14 09:43:27 by jalvarad          #+#    #+#             */
/*   Updated: 2021/11/02 17:56:30 by jalvarad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void close_unnecessary(t_pipe_var info, int a, int b)
{
	int i;

	i = 0;
	while (i < info.size)
	{
		if (info.fd2[i][0] != a && info.fd2[i][0] != b)
			close(info.fd2[i][0]);
		if (info.fd2[i][1] != a && info.fd2[i][1] != b)
			close(info.fd2[i][1]);
		i++;	
	}
}

void	kamikaze_son1(t_pipe_var info, char **argv, char **envp)
{
	if (info.fd1 == -1)
	{
		free(info.path);
		info.path = NULL;
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(": No such file or directory or permission denied\n", 1);
	}
	close_unnecessary(info, info.fd2[0][READ_END], info.fd2[0][WRITE_END]);
	close(info.fd2[0][READ_END]);
	dup2(info.fd2[0][WRITE_END], STDOUT_FILENO);
	close(info.fd2[0][WRITE_END]);
	execve(info.path, argv, envp);
	exit (0);
}

void	kamikaze_sonX(t_pipe_var info, char **argv, char **envp)
{
	if (info.fd1 == -1)
	{
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(": Permission denied\n", 1);
		exit(-1);
	}
	close_unnecessary(info, info.fd2[info.l_p][READ_END], info.fd2[info.n_p][WRITE_END]);
	dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO);
	close(info.fd2[info.l_p][READ_END]);
	//system("lsof -c pipex");
	dup2(info.fd2[info.n_p][WRITE_END], STDOUT_FILENO);
	close(info.fd2[info.n_p][WRITE_END]);
	execve(info.path, argv, envp);
	exit (1);
}

void	kamikaze_son2(t_pipe_var info, char **argv, char **envp)
{
	if (info.fd1 == -1)
	{
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(": Permission denied\n", 1);
		exit(-1);
	}
	close_unnecessary(info, info.fd2[info.l_p][READ_END], -7);
	dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO);
	close(info.fd2[info.l_p][READ_END]);
	execve(info.path, argv, envp);
	exit (1);
}

void	psycho_parent(t_pipe_var info, char **argv, char **envp)
{
	info.pid = fork();
	if (info.pid == -1)
		exit(-1);
	if (info.pid == 0)
		kamikaze_son2(info, argv, envp);
	else
		close(info.fd2[info.l_p][READ_END]);
}


void	save_cmd(t_cmds **stack, char **argv)
{
	t_cmds	*tmp;

	if (*stack == NULL)
		*stack = ft_lstnew(argv);
	else
	{
		tmp = ft_lstnew(argv);
		ft_lstadd_back(stack, tmp);
	}
}

int		**create_doble_array(t_cmds *cmd)
{
	t_cmds	*aux;
	int **pipe_array;
	int		i;
	int		b;
	i = 0;
	aux = cmd;
	while (cmd)
	{
		i++;
		cmd = cmd->next;
	}
	pipe_array = (int **)malloc(sizeof(int *) * (i - 1));
	b = i - 1;
	i = 0;
	while (i < b)
	{
		pipe_array[i] = malloc(sizeof(int) * 2);
		pipe_array[i][0] = -4;
		pipe_array[i][1] = -4;
		i++;
	}
	return (pipe_array);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipe_var	info;
	t_cmds		*cmd;
	t_cmds		*aux;
	char **arg_split;
	char **arg_split2;
	char **arg_split3;
	char **arg_split4;
	int i;

	i = 0;
	cmd = NULL;
	if(argc == 0 || !argv)
		return 0;
	arg_split = ft_mod_split(" cat     ", ' ');
	save_cmd(&cmd, arg_split);
	arg_split2 = ft_mod_split(" cat   ", ' ');
	save_cmd(&cmd, arg_split2); 
	arg_split3 = ft_mod_split("  cat     ", ' ');
	save_cmd(&cmd, arg_split3);
	arg_split4 = ft_mod_split("  echo hola     ", ' ');
	save_cmd(&cmd, arg_split4);
	info.size = ft_lstsize(cmd) - 1;
	//printf("%d\n", info.size);
	info.fd2 = create_doble_array(cmd);
	close (19);
	close (21);
	//system("lsof -c pipex");
	aux = cmd;
	i = 0;
	while (aux)
	{
		if (i < info.size)
			pipe(info.fd2[i]);
		info.path = search_path(aux->content[0], envp);
		if (aux->next)
			info.pid = fork();
		if (info.pid == -1)
			exit(-1);
		if (aux->next && info.pid == 0 && i == 0)
			kamikaze_son1(info, aux->content, envp);
		if (aux->next && i != 0 && info.pid == 0)
			kamikaze_sonX(info, aux->content, envp);
		if (!aux->next && info.pid != 0)
			psycho_parent(info, aux->content, envp);
		if (info.pid != 0) 
		{
			free(info.path);
			if (i < info.size)
			{
				close(info.fd2[i][WRITE_END]);
				if (i > 0)
					close(info.fd2[info.l_p][READ_END]);
			}
		}
		if (info.pid != 0)
		{
			aux = aux->next;
			info.l_p = i;
			i++;
			info.n_p = i;
		}
	}
	while (info.pid != 0 && i > 0)
	{
		wait(&info.status);
		i--;
	}
	return (0);
}
