/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalvarad <jalvarad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/14 09:43:27 by jalvarad          #+#    #+#             */
/*   Updated: 2021/10/31 19:56:09 by jalvarad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	kamikaze_son1(t_pipe_var info, char **argv, char **envp)
{
	//info.fd1 = open(infile, O_RDONLY);
	if (info.fd1 == -1)
	{
		free(info.path);
		info.path = NULL;
		ft_putstr_fd("pipex: ", 1);
		//ft_putstr_fd(infile, 1);
		ft_putstr_fd(": No such file or directory or permission denied\n", 1);
	}
	//printf("%d   %d\n" , info.fd2[0][READ_END], info.fd2[0][WRITE_END]);
	close(info.fd2[0][READ_END]);
	//dup2(info.fd1, STDIN_FILENO);
	//close(info.fd1);
	dup2(info.fd2[0][WRITE_END], STDOUT_FILENO);
	close(info.fd2[0][WRITE_END]);
	rev_and_exe(info.path, argv, envp);
}

void	kamikaze_sonX(t_pipe_var info, char **argv, char **envp)
{
	//info.fd1 = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (info.fd1 == -1)
	{
		ft_putstr_fd("pipex: ", 1);
		//ft_putstr_fd(outfile, 1);
		ft_putstr_fd(": Permission denied\n", 1);
		exit(-1);
	}
	//printf(" puta\n");
	//printf("%s--- %d -- %d\n", info.path, info.l_p, info.n_p);
	dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO);
	close(info.fd2[info.l_p][READ_END]);
	dup2(info.fd2[info.n_p][WRITE_END], STDOUT_FILENO);
	close(info.fd2[info.n_p][WRITE_END]);
	//dup2(info.fd1, STDOUT_FILENO);
	//close(info.fd1);
	if (execve(info.path, argv, envp) == -1)
		exit (0);
}

void	kamikaze_son2(t_pipe_var info, char **argv, char **envp)
{
	//info.fd1 = open("puta", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (info.fd1 == -1)
	{
		ft_putstr_fd("pipex: ", 1);
		//ft_putstr_fd(outfile, 1);
		ft_putstr_fd(": Permission denied\n", 1);
		exit(-1);
	}
	//printf("%s--- %d -- %d\n", info.path, info.l_p, info.n_p);
	dup2(info.fd2[info.l_p][READ_END], STDIN_FILENO);
	close(info.fd2[info.l_p][READ_END]);
	//printf("dndvfvlkdflkvdlk------- %s  %s\n", argv[0], argv[1]);
	
	execve(info.path, argv, envp);
}

void	psycho_parent(t_pipe_var info, char **argv, char **envp)
{
	//printf("jajajjajajaja\n");
	//close(info.fd2[][WRITE_END]);
	info.pid = fork();
	if (info.pid == -1)
		exit(-1);
	if (info.pid == 0)
		kamikaze_son2(info, argv, envp);
	else
	{
		//waitpid(-1, &info.status, 0);
		printf("psycho  %d \n", info.l_p);
		close(info.fd2[info.l_p][READ_END]);
		return;
	}
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
	int i;
	i = 0;
	cmd = NULL;
	if(argc == 0)
		return 0;
	printf("%s\n", argv[1]);
	arg_split = ft_mod_split(" cat ", ' ');
	save_cmd(&cmd, arg_split);
	arg_split2 = ft_mod_split(" echo hola", ' ');
	save_cmd(&cmd, arg_split2); 
	arg_split3 = ft_mod_split(" wc", ' ');
	save_cmd(&cmd, arg_split3);

	info.fd2 = create_doble_array(cmd);
	while (i < 2)
	{
		pipe(info.fd2[i]);
		i++;
	}
	//if (pipe(info.fd2) == -1)
	//	exit(-1);
	aux = cmd;
	//printf("%d---\n", getpid());
	i = 0;
	while (aux)
	{
		info.path = search_path(aux->content[0], envp);
		if (aux->next)
			info.pid = fork();
		if (info.pid == -1)
			exit(-1);
		if (aux->next && info.pid == 0 && i == 0)
		{
			kamikaze_son1(info, aux->content, envp);
		}
		if (aux->next && i != 0 && info.pid == 0)
		{
			kamikaze_sonX(info, aux->content, envp);
		}
		if (!aux->next && info.pid != 0)
		{
			psycho_parent(info, aux->content, envp);
		}
		if (info.pid != 0) 
		{
			free(info.path);
			if (i < 2)
			{
				printf("1 lo de close %d \n", close(info.fd2[i][WRITE_END]));
				if (i > 0)
				{
					printf("2 lo de close %d \n", close(info.fd2[info.l_p][READ_END]));
				}
			}
		}
		aux = aux->next;
		info.l_p = i;
		i++;
		info.n_p = i;
	}
	while (info.pid != 0 && i > 0)
	{
		printf("wait  %d \n",  wait(&info.status));
		i--;
	}
	return (0);
}
