
/*
 *
 * SpiAccessibleApplication function prototypes
 *
 */

/**
 * SpiAccessibleApplication_ref:
 * @obj: a pointer to the #AccessibleApplication on which to operate.
 *
 * Increment the reference count for an #AccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleApplication_ref (SpiAccessibleApplication *obj)
{
  Accessibility_Application_ref (*obj, &ev);
  return 0;
}

/**
 * SpiAccessibleApplication_unref:
 * @obj: a pointer to the #AccessibleApplication object on which to operate.
 *
 * Decrement the reference count for an #AccessibleApplication.
 *
 * Returns: (no return code implemented yet).
 *
 **/
int
SpiAccessibleApplication_unref (SpiAccessibleApplication *obj)
{
  Accessibility_Application_unref (*obj, &ev);
  return 0;
}

/**
 * SpiAccessibleApplication_getToolkitName:
 * @obj: a pointer to the #AccessibleApplication to query.
 *
 * Get the name of the UI toolkit used by an #AccessibleApplication.
 *
 * Returns: a UTF-8 string indicating which UI toolkit is
 *          used by an application.
 *
 **/
char *
SpiAccessibleApplication_getToolkitName (SpiAccessibleApplication *obj)
{
  return Accessibility_Application__get_toolkitName (*obj, &ev);
}

/**
 * SpiAccessibleApplication_getVersion:
 * @obj: a pointer to the #AccessibleApplication being queried.
 *
 * Get the version of the at-spi bridge exported by an
 *      #AccessibleApplication instance.
 *
 * Returns: a UTF-8 string indicating the application's
 *          at-spi version.
 *
 **/
char *
SpiAccessibleApplication_getVersion (SpiAccessibleApplication *obj)
{
  return Accessibility_Application__get_version (*obj, &ev);
}

/**
 * SpiAccessibleApplication_getID:
 * @obj: a pointer to the #AccessibleApplication being queried.
 *
 * Get the unique ID assigned by the SpiRegistry to an
 *      #AccessibleApplication instance.
 * (Not Yet Implemented by the registry).
 *
 * Returns: a unique #long integer associated with the application
 *          by the SpiRegistry, or 0 if the application is not registered.
long
SpiAccessibleApplication_getID (SpiAccessibleApplication *obj)
{
  return Accessibility_Application__get_id (*obj, &ev);
}

/**
 * SpiAccessibleApplication_pause:
 *
 * Attempt to pause the application (used when client event queue is
 *  over-full).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if the application was paused successfully, #FALSE otherwise.
 *
 **/
boolean
SpiAccessibleApplication_pause (SpiAccessibleApplication *obj)
{
  return FALSE;
}

/**
 * SpiAccessibleApplication_resume:
 *
 * Attempt to resume the application (used after #AccessibleApplication_pause).
 * Not Yet Implemented.
 *
 * Returns: #TRUE if application processing resumed successfully, #FALSE otherwise.
 *
 **/
boolean
SpiAccessibleApplication_resume (SpiAccessibleApplication *obj)
{
  return FALSE;
}


