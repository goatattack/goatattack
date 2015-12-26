#include "TextMessageSystem.hpp"

TextMessageSystem::TextMessageSystem() { }

TextMessageSystem::~TextMessageSystem() {
    for (TextMessages::iterator it = text_messages.begin();
        it != text_messages.end(); it++)
    {
        delete *it;
    }
}

void TextMessageSystem::add_text_msg(const std::string& msg) {
    TextMessage *cmsg = new TextMessage;
    cmsg->text = msg;
    text_messages.push_back(cmsg);
}
