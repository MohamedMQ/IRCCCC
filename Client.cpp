/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaqbour <mmaqbour@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:31:12 by rennacir          #+#    #+#             */
/*   Updated: 2023/12/03 16:40:08 by mmaqbour         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.hpp"
#include "Client.hpp"

Client::Client()
{
	this->nickname = "UNKNOWN";
	this->real_name = "UNKNOWN";
	this->username = "UNKNOWN";
	this->is_user = 0;
	this->is_pass = 0;
	this->is_nick = 0;
	this->_retry_pass = 0;
}

void Client::leave_channel(Channel &channel)
{
	this->channels.erase(channel.get_name());
}

int Client::get_is_userF()
{
	return this->is_user;
}

int Client::get_is_nickF() {
	return (this->is_nick);
}

int Client::get_is_passF()
{
	return this->is_pass;
}

void Client::set_is_userF(int a)
{
	this->is_user = a;
}

void Client::set__retry_pass()
{
	this->_retry_pass++;
}

int Client::get__retry_pass()
{
	return this->_retry_pass;
}

void Client::set_is_passF(int a)
{
	this->is_pass = a;
}

void Client::set_is_nickF(int a) {
	this->is_nick = a;
}

void Client::set_is_invited(std::string channel_name)
{
	this->is_invited.push_back(channel_name);
}

int Client::get_is_invited(std::string channel_name)
{
	for (int i = 0; i < this->is_invited.size(); i++)
	{
		if (is_invited[i] == channel_name)
			return 1;
	}
	return 0;
}

void Client::add_channel(Channel &channel, bool a)
{
	this->channels.insert(std::make_pair(channel.get_name(), a));
}

void Client::modify_channel_bool(Channel &channel, bool a)
{
	this->channels[channel.get_name()] = a;
}

int Client::get_channel(Channel &channel)
{
	return this->channels[channel.get_name()];
}

int Client::if_element_exist(Channel &channel)
{
	if (this->channels.count(channel.get_name()) > 0)
		return 1;
	return 0;
}

void Client::set_nickname(std::string nickname)
{
	this->nickname = nickname;
}

std::string Client::get_nickname()
{
	return this->nickname;
}

void Client::set_username(std::string username)
{
	this->username = username;
}

std::string Client::get_username()
{
	return this->username;
}

void Client::set_real_name(std::string realname)
{
	this->real_name = realname;
}

std::string Client::get_real_name()
{
	return this->real_name;
}

void Client::set_private_message(std::string username, std::string message)
{
	int flag = 0;
	for (int i = 0; i < this->private_messages.size(); i++)
	{
		if (this->private_messages[i].user == username)
			flag = 1337;
	}
	if (flag == 1337)
	{
		for (int i = 0; i < this->private_messages.size(); i++)
		{
			if (this->private_messages[i].user == username)
			{
				this->private_messages[i].messages.push_back(message);
				break;
			}
		}
	}
	else
	{
		privmsg prv;
		std::vector<std::string> messages;
		prv.user = username;
		prv.messages = messages;
		prv.messages.push_back(message);
		this->private_messages.push_back(prv);
	}
}

Client::~Client()
{
}
