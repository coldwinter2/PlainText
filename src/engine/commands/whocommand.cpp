#include "whocommand.h"

#include "realm.h"


#define super Command

WhoCommand::WhoCommand(QObject *parent) :
    super(parent) {

    setDescription("Show who is currently online.\n"
                   "\n"
                   "Example: who");
}

WhoCommand::~WhoCommand() {
}

void WhoCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    GameObjectPtrList players = realm()->onlinePlayers();
    if (players.length() == 1) {
        send("Only you are online.");
    } else {
        send(QString("%1 are online.").arg(players.joinFancy(Capitalized)));
    }
}
