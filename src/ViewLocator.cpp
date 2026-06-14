#include "ViewLocator.h"

View **ViewLocator::_carouselViews = nullptr;
ViewLocator::NamedView *ViewLocator::_namedViews = nullptr;
uint8_t ViewLocator::_namedViewCount = 0;
uint8_t ViewLocator::_carouselCount = 0;
uint8_t ViewLocator::_currentCarouselIndex = 0;
View *ViewLocator::_currentView = nullptr;

bool ViewLocator::registerNamedViews(NamedView views[], uint8_t count)
{
  if (_namedViews != nullptr || views == nullptr || count == 0)
  {
    return false;
  }

  for (uint8_t i = 0; i < count; i++)
  {
    if (views[i].view == nullptr)
    {
      return false;
    }

    for (uint8_t j = i + 1; j < count; j++)
    {
      if (views[i].name == views[j].name)
      {
        return false;
      }
    }
  }

  _namedViews = views;
  _namedViewCount = count;
  return true;
}

bool ViewLocator::registerCarouselViews(View *views[], uint8_t count)
{
  if (_carouselViews != nullptr || views == nullptr || count == 0)
  {
    return false;
  }

  for (uint8_t i = 0; i < count; i++)
  {
    if (views[i] == nullptr)
    {
      return false;
    }
  }

  _carouselViews = views;
  _carouselCount = count;
  _currentCarouselIndex = _carouselCount;
  return true;
}

View *ViewLocator::resolveView(ViewName name)
{
  for (uint8_t i = 0; i < _namedViewCount; i++)
  {
    if (_namedViews[i].name != name)
    {
      continue;
    }

    View *targetView = _namedViews[i].view;
    if (_currentView == targetView)
    {
      return _currentView;
    }

    if (_currentView != nullptr)
    {
      _currentView->destroy();
    }

    _currentView = targetView;
    _currentView->initialize();
    return _currentView;
  }

  return nullptr;
}

View *ViewLocator::resolveNextCarouselView()
{
  if (_carouselCount == 0)
  {
    return nullptr;
  }

  uint8_t targetIndex = _currentCarouselIndex >= _carouselCount ? 0 : (_currentCarouselIndex + 1) % _carouselCount;
  View *targetView = _carouselViews[targetIndex];
  _currentCarouselIndex = targetIndex;

  if (_currentView == targetView)
  {
    return _currentView;
  }

  if (_currentView != nullptr)
  {
    _currentView->destroy();
  }

  _currentView = targetView;
  _currentView->initialize();
  return _currentView;
}

View *ViewLocator::resolveCurrentCarouselView()
{
  if (_carouselCount == 0)
  {
    return nullptr;
  }

  if (_currentCarouselIndex >= _carouselCount)
  {
    return nullptr;
  }

  View *targetView = _carouselViews[_currentCarouselIndex];

  if (_currentView == targetView)
  {
    return _currentView;
  }

  if (_currentView != nullptr)
  {
    _currentView->destroy();
  }

  _currentView = targetView;
  _currentView->initialize();
  return _currentView;
}