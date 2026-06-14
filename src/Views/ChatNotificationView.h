#pragma once

#include "../Button.h"
#include "../Display.h"
#include "../IsmChat.h"
#include "../View.h"

/**
 * View that displays the latest ISM chat message notification.
 */
class ChatNotificationView : public View
{
public:
  /**
   * Creates a chat notification view.
   *
   * @param display Display used to render the message.
   * @param enterButton Button used to close the notification.
   * @param ismChat Chat receiver used to read pending messages.
   */
  ChatNotificationView(Display &display, Button &enterButton, IsmChat &ismChat);

  bool update() override;

private:
  void showMessage();

  IsmChat &_ismChat;
  char _message[IsmChat::MaxMessageLength + 1];
};
