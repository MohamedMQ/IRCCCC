/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rennacir <rennacir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:33:51 by rennacir          #+#    #+#             */
/*   Updated: 2023/12/01 21:43:34 by rennacir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_H
#define  CHANNEL_H

#include "irc.hpp"
#include "Client.hpp"

class Client;
typedef struct channel_modes{
	bool i;
	bool t;
	bool k;
	bool o;
	bool l;
} ch_modes;

class Client;

class Channel
{
	private:
		std::vector<std::string> clients;
		std::map<std::string, std::string> messages;
		std::string name;
		ch_modes modes;
		std::string channel_psw;
		std::string channel_topic;
	public:
		Channel();
		void set_channel_psw(std::string psw);
		void set_name(std::string name);
		std::string get_name();
		void set_topic(std::string topic);
		std::string get_topic();
		void set_mode(char mode, int flag);
		ch_modes get_modes();
		void add_client(Client &client);
		void remove_client(Client &client);
		void add_message(std::string username, std::string message);
		// Client get_client(std::string name);
		~Channel();
};

#endif