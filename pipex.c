/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jalvarad <jalvarad@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/14 09:43:27 by jalvarad          #+#    #+#             */
/*   Updated: 2021/10/30 15:00:04 by jalvarad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	kamikaze_son1(t_pipe_var info, char *argv, char **envp, char *infile)
{
	info.fd1 = open(infile, O_RDONLY);
	if (info.fd1 == -1)
	{
		free(info.path);
		info.path = NULL;
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(infile, 1);
		ft_putstr_fd(": No such file or directory or permission denied\n", 1);
	}
	close(info.fd2[READ_END]);
	dup2(info.fd1, STDIN_FILENO);
	close(info.fd1);
	dup2(info.fd2[WRITE_END], STDOUT_FILENO);
	close(info.fd2[WRITE_END]);
	rev_and_exe(info.path, argv, envp);
}

void	kamikaze_son2(t_pipe_var info, char *argv, char **envp, char *outfile)
{
	info.fd1 = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (info.fd1 == -1)
	{
		ft_putstr_fd("pipex: ", 1);
		ft_putstr_fd(outfile, 1);
		ft_putstr_fd(": Permission denied\n", 1);
		exit(-1);
	}
	dup2(info.fd2[READ_END], STDIN_FILENO);
	close(info.fd2[READ_END]);
	dup2(info.fd1, STDOUT_FILENO);
	close(info.fd1);
	rev_and_exe(info.path, argv, envp);
}

void	psycho_parent(t_pipe_var info, char **argv, char **envp)
{
	close(info.fd2[WRITE_END]);
	info.pid = fork();
	if (info.pid == -1)
		exit(-1);
	if (info.pid == 0)
		kamikaze_son2(info, argv[3], envp, argv[4]);
	else
	{
		close(info.fd2[READ_END]);
		wait(&info.status);
	}
}

void	divide_proccess(t_pipe_var info, char **argv, char **envp)
{
	if (info.pid == 0)
		kamikaze_son1(info, argv[2], envp, argv[1]);
	else
	{
		free(info.path);
		info.path = search_path(argv[3], envp);
		psycho_parent(info, argv, envp);
		free(info.path);
	}
}

/*char **remove_quotes(char **arg)
{
	char **mod_arg;

	mod_arg = ;
}*/

char **treatment_args(char *arg)
{
	//char	**mod_argv;
	//int		size;
	int		j;
	//int		smp_qts; // comillas simples
	int		qts; // comillas dobles
	int i;
	
	qts = 0;
	//size = 0;
	i = 0;
	j = 0;
	while (arg[i])
	{
		while (arg[i] && qts % 2 == 0)
		{
			if (arg[i] == '\'' || arg[i] == '"')
				qts++;
			i++;
		}
	}
	printf("%d\n", j);
	//mod_argv = (char **)malloc(sizeof(char *) * (i + 1));
	//if (!mod_argv)
		//printf("error en la memoria\n"); //// falta hacer mensajes de error
	return (NULL);
}




int	main(int argc, char **argv, char **envp)
{
	t_pipe_var	info;
	char **arg_split; 
	int i;
	i = 0;
	if(argc == 0)
		return 0;
	printf("%s\n", argv[1]);
	//printf("%d\n", mod_word_count("    ' \" jjaja   fnbkfn   ijgjg\"  ' ijffkk    ", ' '));
	arg_split = ft_mod_split(" a   \" jjaja   fnbkfn   ijgjg\" puta \"ijffkk \"    ", ' ');
	while (arg_split[i])
	{
		printf("|%s|\n", arg_split[i]);
		i++;
	}
	exit (-1);
	pipe(info.fd2);
	if (pipe(info.fd2) == -1)
		exit(-1);
	info.path = search_path(argv[2], envp);
	info.pid = fork();
	if (info.pid == -1)
		exit(-1);
	divide_proccess(info, argv, envp);
	wait(&info.status);
	return (0);
}
