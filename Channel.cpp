/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rennacir <rennacir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:55:33 by rennacir          #+#    #+#             */
/*   Updated: 2023/12/01 21:47:41 by rennacir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
{
	this->channel_psw = "";
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
		this->modes.t = flag;
	else if (mode == 'o')
		this->modes.t = flag;
	else if (mode == 'i')
		this->modes.t = flag;
	else if (mode == 'l')
		this->modes.t = flag;
}

void Channel::add_message(std::string username, std::string message)
{
	this->messages[username] = message;
}

// Client Channel::get_client(std::string name)
// {
// 	for (int i = 0; i < this->clients.size() ; i++)
// 	{
// 		if(name == this->clients[i].get_nickname())
// 			return this->clients[i];
// 	}
// }

Channel::~Channel()
{

}
