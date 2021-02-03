// Copyright (C) <2018> Intel Corporation
//
// SPDX-License-Identifier: Apache-2

#pragma once

#include <vector>
#include "sio_client.h"

#include "owt/p2p/p2psignalingchannelinterface.h"

using namespace owt::p2p;

class connection_listener
{
	sio::client &handler;
public:
	connection_listener(sio::client& hdl) : handler(hdl) {}

	void on_connected()	{
        LOG(L"ConnectSuccess");
        if (target_id.size() == 0) {
            return;
        }
        sio::message::ptr jsonObject = sio::object_message::create();
        jsonObject->get_map()["token"] = sio::string_message::create(target_id);
        handler.socket()->emit("authentication", jsonObject, [](const sio::message::list& msg) {
            if (msg.size() > 0 && msg[0]->get_flag() == sio::message::flag_integer) {
                //on_fail
                int64_t error_code = msg[0]->get_int();
                return;
            }

            if (msg[0]->get_flag() == sio::message::flag_object) {
                sio::message::flag uidFlag = msg[0]->get_flag();
            }
            //on_success
        });
    }

    void setTargetId(const std::string& target_id) {
        this->target_id = target_id;
    }
private:
    std::string target_id;
};

class P2PSocketSignalingChannel : public P2PSignalingChannelInterface {
public:
    explicit P2PSocketSignalingChannel();
    virtual void AddObserver(
		P2PSignalingChannelObserver& observer) override;
    virtual void RemoveObserver(
		P2PSignalingChannelObserver& observer) override;
    virtual void Connect(const std::string& host,
        const std::string& token,
        std::function<void(const std::string&)> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure) override;
    virtual void Disconnect(std::function<void()> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure) override;
    virtual void SendMessage(const std::string& message,
        const std::string& target_id,
        std::function<void()> on_success,
        std::function<void(std::unique_ptr<Exception>)> on_failure) override;

private:
	// Observers bind to this signaling channel instance
    std::vector<P2PSignalingChannelObserver*> observers_;
    std::unique_ptr<sio::client> io_;
    std::function<void(const std::string&)> connect_success_callback_;
    std::unique_ptr<connection_listener> connection_listener_;
};
