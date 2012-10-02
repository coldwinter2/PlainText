#ifndef USECOMMAND_H
#define USECOMMAND_H

#include "command.h"


class UseCommand : public Command {

    Q_OBJECT

    public:
        UseCommand(QObject *parent = 0);
        virtual ~UseCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // USECOMMAND_H
