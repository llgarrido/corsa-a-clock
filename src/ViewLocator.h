#pragma once

#include <Arduino.h>
#include "View.h"

/**
 * @brief Identifiers for views that can be resolved directly by name.
 */
enum class ViewName : uint8_t
{
  /**
   * @brief Initial splash screen shown during startup.
   */
  SplashScreen
};

/**
 * @brief Static registry and resolver for application views.
 *
 * ViewLocator stores named views and carousel views, initializes the resolved
 * view, and destroys the previously active view before switching.
 */
class ViewLocator
{
public:
  /**
   * @brief Pair that links a view name to its view instance.
   */
  struct NamedView
  {
    /**
     * @brief Identifier used to resolve the view later.
     */
    ViewName name;

    /**
     * @brief View instance associated with the name.
     */
    View *view;
  };

  /**
   * @brief Prevents creating instances because ViewLocator is a static service.
   */
  ViewLocator() = delete;

  /**
   * @brief Registers the views used by the carousel.
   *
   * Registration fails when carousel views have already been registered, when
   * views is null, when count is zero, or when any view pointer is null.
   *
   * ViewLocator stores a pointer to the registration array, so the array and
   * the view instances referenced by it must remain alive while ViewLocator can
   * resolve them. Use static or global storage for the registration array.
   *
   * @param views Array of view pointers to cycle through.
   * @param count Number of entries in views.
   * @return true if the carousel views were registered; otherwise false.
   */
  static bool registerCarouselViews(View *views[], uint8_t count);

  /**
   * @brief Registers the views that can be resolved by name.
   *
   * Registration fails when named views have already been registered, when
   * views is null, when count is zero, when any view pointer is null, or when
   * the same name appears more than once.
   *
   * ViewLocator stores a pointer to the registration array, so the array and
   * the view instances referenced by it must remain alive while ViewLocator can
   * resolve them. Use static or global storage for the registration array.
   *
   * @param views Array of named view registrations.
   * @param count Number of entries in views.
   * @return true if the named views were registered; otherwise false.
   */
  static bool registerNamedViews(NamedView views[], uint8_t count);

  /**
   * @brief Resolves and activates a named view.
   *
   * When switching to a different view, the previously active view is
   * destroyed before the resolved view is initialized.
   *
   * @param name Identifier of the view to activate.
   * @return Pointer to the active view, or nullptr if no view is registered.
   */
  static View *resolveView(ViewName name);

  /**
   * @brief Resolves and activates the next carousel view.
   *
   * When switching to a different view, the previously active view is
   * destroyed before the next carousel view is initialized.
   *
   * @return Pointer to the active view, or nullptr if the carousel is empty.
   */
  static View *resolveNextCarouselView();

private:
  static View **_carouselViews;
  static NamedView *_namedViews;
  static uint8_t _namedViewCount;
  static uint8_t _carouselCount;
  static uint8_t _currentCarouselIndex;
  static View *_currentView;
};
