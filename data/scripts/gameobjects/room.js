
function Room() {
}

Room.prototype.addGold = function(amount) {

    var goldItem;
    for (var i = 0, length = this.items.length; i < length; i++) {
        var item = this.items[i];
        if (item.name.endsWith("worth of gold")) {
            goldItem = item;
            break;
        }
    }

    if (goldItem === null) {
        goldItem = Realm.createObject("Item");
        goldItem.cost = amount;
        goldItem.portable = true;
        this.addItem(goldItem);
    } else {
        goldItem.cost += amount;
    }

    goldItem.name = "$%1 worth of gold".arg(amount);
}