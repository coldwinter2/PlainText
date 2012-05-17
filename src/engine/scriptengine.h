#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>

#include "gameobjectptr.h"
#include "scriptfunction.h"


class Area;
class Character;

class ScriptEngine : public QObject {

    Q_OBJECT

    public:
        static ScriptEngine *instance() { Q_ASSERT(s_instance); return s_instance; }

        static void instantiate();
        static void destroy();

        bool isInitialized() const { return m_initialized; }

        QScriptValue evaluate(const QString &program, const QString &fileName = QString(), int lineNumber = 1);
        ScriptFunction defineFunction(const QString &program, const QString &fileName = QString(), int lineNumber = 1);

        bool hasUncaughtException() const;

        bool executeFunction(ScriptFunction &function, const GameObjectPtr &thisObject,
                             const GameObjectPtrList &objects);
    private:
        static ScriptEngine *s_instance;

        bool m_initialized;

        QScriptEngine m_jsEngine;

        explicit ScriptEngine();
        virtual ~ScriptEngine();
};

#endif // SCRIPTENGINE_H