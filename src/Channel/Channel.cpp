
#include "Channel/Channel.hpp"

Channel::Channel(const std::string& name) : _name(name), _topic(""), _key(""),
	_inviteOnly(false), _topicRestricted(false)
{}

Channel::~Channel() {}

bool Channel::hasUser(Client* client) const
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
			return true;
	}
	return false;
}

bool Channel::addUser(Client* client, const std::string& key)
{
	if (hasUser(client))
		return false;

	if (_hasLimit && _clients.size() >= _userLimit)
		return false;

	if (!_key.empty() && _key != key)
		return false;

	if (_inviteOnly && !isInvited(client))
		return false;

	_clients.push_back(client);
	removeInvite(client);
	return true;
}

bool Channel::removeUser(Client* client)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			_clients.erase(it);
			removeOperator(client);
			return true;
		}
	}
	return false;
}

bool Channel::isOperator(Client* client) const
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client)
			return true;
	}
	return false;
}

void Channel::addOperator(Client* client)
{
	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client* client)
{
	for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it == client)
		{
			_operators.erase(it);
			return;
		}
	}
}

void Channel::inviteClient(Client* op, Client* target)
{
	if (!isOperator(op))
		return;

	if (!isInvited(target))
		_invitedClients.push_back(target);
}

bool Channel::isInvited(Client* client) const
{
	for (size_t i = 0; i < _invitedClients.size(); i++)
	{
		if (_invitedClients[i] == client)
			return true;
	}
	return false;
}

void Channel::removeInvite(Client* client)
{
	for (std::vector<Client*>::iterator it = _invitedClients.begin(); it != _invitedClients.end(); ++it)
	{
		if (*it == client)
		{
			_invitedClients.erase(it);
			return;
		}
	}
}

bool Channel::kickClient(Client* op, Client* target)
{
	if (!isOperator(op) || !hasUser(target))
		return false;

	removeUser(target);
	return true;
}

bool Channel::setTopic(Client* client, const std::string& topic)
{
	if (_topicRestricted && !isOperator(client))
		return false;

	_topic = topic;
	return true;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

void Channel::setInviteOnly(bool enabled)
{
	_inviteOnly = enabled;
}

void Channel::setTopicRestriction(bool enabled)
{
	_topicRestricted = enabled;
}

void Channel::setKey(const std::string& key)
{
	_key = key;
}

void Channel::removeKey()
{
	_key.clear();
}

bool Channel::checkKey(const std::string& key) const
{
	return (_key == key);
}

void Channel::broadcast(Client* sender, const std::string& message)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] != sender)
		{
			//_clients[i]->sendMessage(message);
			std::cout << message << std::endl;
		}
	}
}

const std::string& Channel::getName() const
{
	return _name;
}

size_t Channel::getUserCount() const
{
	return _clients.size();
}