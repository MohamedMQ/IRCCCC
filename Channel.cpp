/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rennacir <rennacir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:55:33 by rennacir          #+#    #+#             */
/*   Updated: 2023/12/05 14:25:49 by rennacir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
{
	this->channel_psw = "";
}

int Channel::get_num_of_clients()
{
	return (this->clients.size());
}

int Channel::get_limit_num_of_clients()
{
	return this->client_limit;
}

void Channel::set_limit_num_of_clients(int l)
{
	this->client_limit = l;
}

void Channel::set_name(std::string name)
{
	this->name = name;
}

std::string Channel::get_name()
{
	return this->name;
}

void Channel::set_topic(std::string topic)
{
	this->channel_topic = topic;
}

std::string Channel::get_topic()
{
	return this->channel_topic;
}

void Channel::set_channel_psw(std::string psw)
{
	this->channel_psw = psw;
}

std::string Channel::get_channel_psw()
{
	return this->channel_psw;
}

ch_modes Channel::get_modes()
{
	return this->modes;
}

void Channel::add_client(Client &client)
{
	this->clients.push_back(client.get_nickname());
}

void Channel::remove_client(Client &client)
{
	for (int i = 0; i < this->clients.size() ; i++)
	{
		if (this->clients[i] == client.get_nickname())
			this->clients.erase(this->clients.begin() + i);
	}
}

void Channel::set_mode(char mode, int flag)
{
	if (mode == 't')
		this->modes.t = flag;
	else if (mode == 'k')
		this->modes.k = flag;
	else if (mode == 'o')
		this->modes.o = flag;
	else if (mode == 'i')
		this->modes.i = flag;
	else if (mode == 'l')
		this->modes.l = flag;
}

void Channel::add_message(std::string username, std::string message)
{
	int flag = 0;
	for (int i = 0; i < this->channel_private_messages.size() ; i++)
	{
		if (this->channel_private_messages[i].user == username)
		{
			flag = 1337;
			break;
		}
	}
	if (flag == 1337)
	{
		for (int i = 0; i < this->channel_private_messages.size() ; i++)
		{
			if (this->channel_private_messages[i].user == username)
			{
				this->channel_private_messages[i].messages.push_back(message);
				break;
			}
		}
	}
	else
	{
		privmsg prv;
		prv.user = username;
		prv.messages.push_back(message);
		this->channel_private_messages.push_back(prv);
	}
	// this->messages[username] = message;
}

Channel::~Channel()
{

}
