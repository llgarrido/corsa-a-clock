#include "ChatNotificationView.h"

ChatNotificationView::ChatNotificationView(Display &display, Button &enterButton, IsmChat &ismChat)
    : View(display, enterButton), _ismChat(ismChat)
{
  _message[0] = '\0';
}

void ChatNotificationView::showMessage()
{
  if (!_ismChat.readMessage(_message, sizeof(_message)))
    return;

  drawBackground();
  display().write(0, 0, "MSG:");
  display().write(4, 0, _message, true);
}

bool ChatNotificationView::update()
{
  if (_ismChat.hasMessage())
  {
    showMessage();
    return true;
  }

  return !enterButton().pressed();
}
