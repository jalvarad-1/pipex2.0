/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalvarad <jalvarad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/14 09:43:27 by jalvarad          #+#    #+#             */
/*   Updated: 2021/11/01 17:53:44 by jalvarad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	kamikaze_son1(t_pipe_var info, char **argv, char **envp)
{
	if (info.fd1 == -1)
	{
		free(info.path);
		info.path = NULL;
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(": No such file or directory or permission denied\n", 1);
	}
	printf("son 1: se cierra %d %d\n", info.fd2[0][READ_END], info.fd2[0][WRITE_END]);
	close(info.fd2[0][READ_END]);
	printf("1 dupp %d\n",dup2(info.fd2[0][WRITE_END], STDOUT_FILENO));
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
	printf("son 1,5: cierra %d %d\n", info.fd2[info.l_p][READ_END], info.fd2[info.n_p][WRITE_END]);
	printf("1,5 read dup  %d\n",dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO));
	close(info.fd2[info.l_p][READ_END]);
	printf("1,5d write up %d\n",dup2(info.fd2[info.n_p][WRITE_END], STDOUT_FILENO));
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
	printf("son 2: cierra %d \n", info.fd2[info.l_p][READ_END]);
	//printf("kamikase 2  %s--- %d -- %d\n", info.path, info.l_p, info.n_p);
	printf("2 dup %d\n", dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO));
	close(info.fd2[info.l_p][READ_END]);
	//printf("puta||||\n");
	execve(info.path, argv, envp);
	exit (1);
}

void	psycho_parent(t_pipe_var info, char **argv, char **envp)
{
	info.pid = fork();
	printf("extra process %d \n", getpid());
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
	//char **arg_split4;
	//char **arg_split5;
	int i;
	int b; /// tamaño del array de pipes
	i = 0;
	cmd = NULL;
	if(argc == 0 || !argv)
		return 0;
	arg_split = ft_mod_split(" cat  ", ' ');
	save_cmd(&cmd, arg_split);
	arg_split2 = ft_mod_split(" cat ", ' ');
	save_cmd(&cmd, arg_split2); 
	arg_split3 = ft_mod_split("  echo hoola ", ' ');
	save_cmd(&cmd, arg_split3);
	/*arg_split4 = ft_mod_split("  wc ", ' ');
	save_cmd(&cmd, arg_split4);
	arg_split5 = ft_mod_split(" grep 6 ", ' ');
	save_cmd(&cmd, arg_split5);*/
	b = ft_lstsize(cmd) - 1;
	printf("%d\n", b);
	//exit (0);
	info.fd2 = create_doble_array(cmd);
	/*while (i < b)
	{
		pipe(info.fd2[i]);
		printf("  %d ,  %d \n", info.fd2[i][READ_END], info.fd2[i][WRITE_END]);
		i++;
	}*/
	aux = cmd;
	i = 0;
	while (aux)
	{
		if (i < b)
			pipe(info.fd2[i]);
		info.path = search_path(aux->content[0], envp);
		if (aux->next)
		{
			//printf("padre----->%d\n", getpid());
			info.pid = fork();
			//printf("entrooooooo----->%d\n", getpid());
		}
		if (info.pid == -1)
			exit(-1);
		if (aux->next && info.pid == 0 && i == 0)
		{
			printf("son 1: path %s -- PID (%d)\n", info.path, getpid());
			kamikaze_son1(info, aux->content, envp);
		}
		if (aux->next && i != 0 && info.pid == 0)
		{
			//printf("son x: path %s -- PID (%d)\n", info.path, getpid());
			kamikaze_sonX(info, aux->content, envp);
		}
		if (!aux->next && info.pid != 0)
		{
			//printf("son 2: path %s -- PID (%d)\n", info.path, getpid());
			psycho_parent(info, aux->content, envp);
		}
		if (info.pid != 0) 
		{
			free(info.path);
			if (i < b)
			{
				sleep(10);
				printf(" se cierra escritura %d\n" ,info.fd2[i][WRITE_END]);
				close(info.fd2[i][WRITE_END]);
				if (i > 0)
				{
					printf(" se cierra lectura %d\n" ,info.fd2[info.l_p][READ_END]);
					close(info.fd2[info.l_p][READ_END]);
				}
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
		printf ("ha llegado %d\n", wait(&info.status));
		i--;
	}
	system("lsof -c pipex");
	return (0);
}
