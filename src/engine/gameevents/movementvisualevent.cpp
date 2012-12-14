#include "movementvisualevent.h"

#include "character.h"
#include "point3d.h"
#include "portal.h"
#include "room.h"
#include "util.h"
#include "vector3d.h"


#define super VisualEvent

MovementVisualEvent::MovementVisualEvent(Room *origin, double strength) :
    super(origin, strength),
    m_destination(nullptr) {
}

MovementVisualEvent::~MovementVisualEvent() {
}

void MovementVisualEvent::setDestination(Room *destination) {

    m_destination = destination;

    addVisit(destination, strengthForRoom(originRoom()));
}

void MovementVisualEvent::setMovement(const Vector3D &movement) {

    m_movement = movement;
}

void MovementVisualEvent::setDirection(const Vector3D &direction) {

    m_direction = direction;
}

void MovementVisualEvent::setVerb(const QString &simplePresent, const QString &continuous) {

    m_simplePresent = simplePresent;
    m_continuous = continuous;
}

QString MovementVisualEvent::descriptionForStrengthInRoom(double strength, Room *room) const {

    if (room == originRoom()) {
        QString characterName = Util::capitalize(description());
        for (const GameObjectPtr &portalPtr : room->portals()) {
            Portal *portal = portalPtr.cast<Portal *>();
            if (portal->room() == m_destination || portal->room2() == m_destination) {
                QString exitName = portal->nameFromRoom(room);
                if (Util::isDirection(exitName)) {
                    return QString("%1 %2 %3.").arg(characterName, m_simplePresent, exitName);
                } else if (exitName == "out") {
                    return QString("%1 %2 outside.");
                } else {
                    if (portal->canOpenFromRoom(room)) {
                        return QString("%1 %2 through the %3.").arg(characterName, m_simplePresent,
                                                                    exitName);
                    } else {
                        return QString("%1 %2 to the %3.").arg(characterName, m_simplePresent,
                                                               exitName);
                    }
                }
            }
        }

        return QString("%1 %2 %3.").arg(characterName, m_simplePresent,
                                        Util::directionForVector(m_direction));
    } else if (room == m_destination) {
        return QString("%1 %2 to you.").arg(Util::capitalize(description()), m_simplePresent);
    }

    QString direction;
    Vector3D vector = room->position() - originRoom()->position();

    if (vector.length() > 100) {
        double angle = m_direction.angle(vector);
        if (angle < TAU / 8) {
            direction = "in your direction";
        } else {
            direction = Util::directionForVector(m_direction);
        }
        return QString("In the distance, you see %1 %2 %3.")
                .arg(strength > 0.9 ? description() :
                     strength > 0.8 ? distantDescription() :
                     veryDistantDescription(), m_continuous, direction);
    } else {
        double angle = m_direction.angle(vector);
        if (angle < TAU / 8) {
            direction = "toward you";
        } else if (angle >= TAU * 3 / 8) {
            direction = "away from you";
        } else {
            direction = Util::directionForVector(m_direction);
        }
        return QString("You see %1 %2 %3.").arg(strength > 0.9 ? description() :
                                                strength > 0.8 ? distantDescription() :
                                                veryDistantDescription(), m_continuous, direction);
    }
}

bool MovementVisualEvent::isWithinSight(Room *targetRoom, Room *sourceRoom) {

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
