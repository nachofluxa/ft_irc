
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server/Server.hpp"
#include "Client/Client.hpp"
#include <vector>
#include <string>
#include <iterator>
#include <iostream>

class Channel
{
	private:
		std::string				_name;
		std::string				_topic;
		std::string				_key;
		std::vector<Client*>	_clients;
		std::vector<Client*>	_operators;
		std::vector<Client*>	_invitedClients;
		bool					_inviteOnly;
		bool					_topicRestricted;
		bool					_hasLimit;
		size_t					_userLimit;

	public:
		Channel(const std::string& name);
		~Channel();

		// USERS
		bool hasUser(Client* client) const;
		bool addUser(Client* client, const std::string& key);
		bool removeUser(Client* client);

		// OPERATORS
		bool isOperator(Client* client) const;
		void addOperator(Client* client);
		void removeOperator(Client* client);

		// INVITE
		void inviteClient(Client* op, Client* target);
		bool isInvited(Client* client) const;
		void removeInvite(Client* client);

		// KICK 
		bool kickClient(Client* op, Client* target);

		// TOPIC
		bool setTopic(Client* client, const std::string& topic);
		const std::string& getTopic() const;

		// MODE
		void setInviteOnly(bool enabled);
		void setTopicRestriction(bool enabled);
		void setKey(const std::string& key);
		void removeKey();
		bool checkKey(const std::string& key) const;

		// MESSAGING
		void broadcast(Client* sender, const std::string& message);

		// GETTERS
		const	std::string&	getName() const;
		size_t	getUserCount() const;
};

#endif