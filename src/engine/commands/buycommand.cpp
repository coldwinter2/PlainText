#include "buycommand.h"

#include "engine/util.h"


BuyCommand::BuyCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription(QString("Buy an item from a character. Use %1 to enquiry what goods a \n"
                           "character has for sale. Use %2 to buy a specific item \n"
                           "from a character.\n"
                           "\n"
                           "Examples: buy from shopkeeper, buy book from shopkeeper")
                   .arg(Util::highlight("buy from <character>"))
                   .arg(Util::highlight("buy <item> from <character>")));
}

BuyCommand::~BuyCommand() {
}

void BuyCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList sellers;
    for (const GameObjectPtr &npc : currentArea()->npcs()) {
        if (npc.cast<Character *>()->sellableItems().length() > 0) {
            sellers << npc;
        }
    }

    if (!requireSome(sellers, "There is noone that sells anything here.")) {
        return;
    }

    if (!assertWordsLeft("Buy from who?")) {
        return;
    }

    QString word = takeWord();

    QPair<QString, uint> itemDescription;
    GameObjectPtr character;

    if (word == "the") {
        if (!assertWordsLeft("Buy what?")) {
            return;
        }

        itemDescription = takeObjectsDescription();
    } else if (word == "from") {
        takeWord("the");

        if (!assertWordsLeft("Buy from who?")) {
            return;
        }

        character = takeObject(currentArea()->npcs());
    } else {
        appendWord(word);

        itemDescription = takeObjectsDescription();
    }

    if (character.isNull()) {
        if (hasWordsLeft()) {
            takeWord("from");
            takeWord("the");

            if (!assertWordsLeft("Buy from who?")) {
                return;
            }

            character = takeObject(currentArea()->npcs());
        } else if (!itemDescription.first.isEmpty()) {
            GameObjectPtrList characters = objectsByDescription(itemDescription,
                                                                currentArea()->npcs());
            if (characters.length() > 0) {
                character = characters[0];
                itemDescription = QPair<QString, uint>();
            }
        }

        if (character.isNull()) {
            if (sellers.length() == 1) {
                character = sellers[0];
            } else {
                player()->send("Buy from who?");
                return;
            }
        }
    }

    if (!requireSome(character, "That character is not here.")) {
        return;
    }

    Character *seller = character.cast<Character *>();
    QString sellerName = seller->definiteName(currentArea()->npcs(), GameObject::Capitalized);

    if (seller->sellableItems().length() == 0) {
        player()->send(QString("%1 has nothing for sale.").arg(sellerName));
        return;
    }

    if (itemDescription.first.isEmpty()) {
        seller->invokeTrigger("onbuy", player());
        return;
    }

    GameObjectPtrList items = objectsByDescription(itemDescription, seller->sellableItems());
    if (!requireSome(items, QString("%1 doesn't sell that.").arg(sellerName))) {
        return;
    }

    Item *item = items[0].cast<Item *>();
    if (item->cost() > player()->gold()) {
        player()->send("You don't have enough gold to buy that.");
        return;
    }

    if (!seller->invokeTrigger("onbuy", player(), items[0])) {
        return;
    }

    player()->adjustGold(-item->cost());
    player()->addInventoryItem(item->copy());

    player()->send(QString("You bought %1.").arg(item->indefiniteName()));
}