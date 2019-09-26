#pragma once
#ifndef MX_APPLICATION_BASE_H_
#define MX_APPLICATION_BASE_H_
#include <string>
#include "Mx/Definitions/MxVersion.h"
#include <vector>

namespace Mix {
    struct ApplicationSetting {
        std::string appName;
        Version appVersion;
    };

    class MixEngine;

    class ApplicationBase {
        friend MixEngine;

    public:
        ApplicationBase(ApplicationSetting _setting);

        virtual ~ApplicationBase() = default;

        /**
         * \brief Set the maximum frames per second, zero means no limits.
         */
        void setFPSLimit(uint32_t _limit);

        /**
         * \brief Call to issue a request for the application to close. \n
         *        This will eventually trigger an quit event and onQuitRequested() will be called.
         * \note  ONLY call this after startUp() has been called
         */
        void requestQuit();

        /** \brief Get the name of the application */
        const std::string& getAppName() const { return mSetting.appName; }

        /** \brief Get the version of the application */
        const Version& getAppVersion() const { return mSetting.appVersion; }

    protected:
        /**
        * \brief Called before engine starts up.
        * \param _args Arguments from command line.
        * \note Engine has not been initialized when this function is called.
        */
        virtual void startUp(const std::vector<std::string>& _args);

        /** \brief Called after modules are loaded */
        virtual void onModuleLoaded();

        /** \brief Called after modules are initialized */
        virtual void onMoudleInitialized();

        /** \brief Called right after the main scene is created*/
        virtual void onMainSceneCreated();

        /** \brief Called in each main loop before any SceneObjects are updated. */
        virtual void onUpdate();

        /** \brief Called during fixed update in the main loop. Called after update and before late update. */
        virtual void onFixedUpdate();

        /** \brief Called in each main loop after all SceneObjects are updated. Called after fixed update */
        virtual void onLateUpdate();

        /** \brief Called before any render operations begin */
        virtual void onRender();

        /** \brief Called after all render operations are done */
        virtual void onPostRender();

        /**
         * \brief Called when a quit event is issued.
         * \return True if application wants to quit.
         * \note  It DOES NOT mean that application will quit when this is called. Instead,\n
         *        it's just a request, it's up to application to decide whether to quit or not.
         */
        virtual bool onQuitRequested();

        /** \brief Called when main loop ends and the application is going to exit.*/
        virtual void onQuit();

    protected:
        ApplicationSetting mSetting;
    };
}

#endif
