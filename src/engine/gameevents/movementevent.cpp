#include "movementevent.h"

#include "character.h"
#include "point3d.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super VisualEvent

MovementEvent::MovementEvent(Character *character, Room *origin, double strength) :
    super(origin, strength),
    m_character(character),
    m_destination(nullptr) {
}

MovementEvent::~MovementEvent() {
}

void MovementEvent::setDestination(Room *destination) {

    m_destination = destination;

    addVisit(destination, strengthForRoom(originRoom()));
}

void MovementEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

QString MovementEvent::descriptionForStrengthInRoom(double strength, Room *room) const {

    Q_UNUSED(room)

    QString direction = Util::directionForVector(m_direction);

    if (strength > 0.9) {
        return QString("You see %1 walking to the %2.").arg(m_character->name(), direction);
    } else if (strength > 0.8) {
        QString sex = m_character->gender() == "male" ? "a man" : "a woman";
        return QString("You see %1 walking to the %2.").arg(sex, direction);
    } else if (strength > 0.6) {
        return QString("You see someone walking to the %1.").arg(direction);
    } else {
        return QString("You see a shape moving to the %1.").arg(direction);
    }
}

bool MovementEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

    if (sourceRoom == originRoom() || sourceRoom == m_destination) {
        return true;
    }

    Vector3D sourceVector = (sourceRoom->position() - originRoom()->position()).normalized();
    Vector3D targetVector = (targetRoom->position() - sourceRoom->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (sourceRoom->flags() & RoomFlags::NoCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (sourceRoom->flags() & RoomFlags::NoFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    sourceVector = (sourceRoom->position() - m_destination->position()).normalized();
    if (sourceVector == targetVector) {
        return true;
    }

    if (sourceRoom->flags() & RoomFlags::NoCeiling) {
        if (targetVector.z >= sourceVector.z) {
            return true;
        }
    }
    if (sourceRoom->flags() & RoomFlags::NoFloor) {
        if (targetVector.z <= sourceVector.z) {
            return true;
        }
    }

    return false;
}