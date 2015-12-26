#ifndef _TEXTMESSAGESYSTEM_HPP_
#define _TEXTMESSAGESYSTEM_HPP_

#include <string>
#include <vector>

class TextMessageSystem {
public:
    TextMessageSystem();
    virtual ~TextMessageSystem();

    void add_text_msg(const std::string& msg);

protected:
    struct TextMessage {
        TextMessage() : duration(0.0f), delete_me(false) { }
        std::string text;
        double duration;
        bool delete_me;
    };

    typedef std::vector<TextMessage *> TextMessages;

    TextMessages text_messages;
};


#endif