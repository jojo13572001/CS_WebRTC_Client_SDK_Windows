// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include "p2psocketsignalingchannel.h"

P2PSocketSignalingChannel::P2PSocketSignalingChannel()
    : io_(new sio::client()), connection_listener_(new connection_listener(*io_)) {}

void P2PSocketSignalingChannel::AddObserver(
	P2PSignalingChannelObserver& observer) {
    observers_.push_back(&observer);
}

void P2PSocketSignalingChannel::RemoveObserver(
	P2PSignalingChannelObserver& observer) {
    observers_.erase(remove(observers_.begin(), observers_.end(), &observer),
        observers_.end());
}

void P2PSocketSignalingChannel::Connect(
    const std::string& host,
    const std::string& token,
    std::function<void(const std::string&)> on_success,
    std::function<void(std::unique_ptr<Exception>)> on_failure) {
    connect_success_callback_ = on_success;

    std::map<std::string, std::string> query;
    query.insert(std::pair<std::string, std::string>("clientVersion", "5.0"));
    query.insert(std::pair<std::string, std::string>("clientType", "cpp"));
    query.insert(std::pair<std::string, std::string>("token", token)); // TODO: parse token to get actual token.

    connection_listener_->setTargetId(token);
    io_->set_open_listener(std::bind(&connection_listener::on_connected, connection_listener_.get()));

    sio::socket::ptr socket = io_->socket();
    std::string ics_chat = "owt-message";
    std::string auth_done = "server-authenticated";
    socket->on(
        ics_chat,
        sio::socket::event_listener_aux(
            [&](std::string const& name, sio::message::ptr const& data,
                bool has_ack, sio::message::list& ack_resp) {
                if (data->get_flag() == sio::message::flag_object) {
                    std::string msg = data->get_map()["data"]->get_string().data();
                    std::string from = data->get_map()["from"]->get_string().data();
                    for (auto it = observers_.begin(); it != observers_.end(); ++it)
                        (*it)->OnSignalingMessage(msg, from);
                }
            }));
	  socket->on(
        auth_done,
        sio::socket::event_listener_aux(
            [&](std::string const& name, sio::message::ptr const& data,
			          bool has_ack, sio::message::list& ack_resp) {
                if (data->get_flag() == sio::message::flag_object && connect_success_callback_) {
                    std::string msg = data->get_map()["uid"]->get_string().data();
                    connect_success_callback_(msg);
		            }
	          }));
    io_->connect(host, query);
}

void P2PSocketSignalingChannel::Disconnect(std::function<void()> on_success,
    std::function<void(std::unique_ptr<Exception>)> on_failure) {
  if (!io_->opened()) {
    if (on_failure) {
      std::unique_ptr<Exception> exception(
        new Exception(ExceptionType::kP2PClientInvalidState,
          "Socket IO service has not been opened yet."));
      on_failure(std::move(exception));
    }
    return;
  }

  io_->close();
}

void P2PSocketSignalingChannel::SendMessage(
    const std::string& message,
    const std::string& target_id,
    std::function<void()> on_success,
    std::function<void(std::unique_ptr<Exception>)> on_failure) {
    sio::message::ptr jsonObject = sio::object_message::create();
    jsonObject->get_map()["to"] = sio::string_message::create(target_id);
    jsonObject->get_map()["data"] = sio::string_message::create(message);
    io_->socket()->emit("owt-message", jsonObject, [=](const sio::message::list& msg) {
        if (msg.size() > 0 && msg[0]->get_flag() == sio::message::flag_integer) {
              int64_t error_code = msg[0]->get_int();
              if (on_failure) {
                std::unique_ptr<Exception> exception(
                  new Exception(ExceptionType::kP2PMessageTargetUnreachable,
                    "Remote user cannot be reached."));
                on_failure(std::move(exception));
              }
              return;
        }

        if (on_success) {
            on_success();
        }
    });
}
