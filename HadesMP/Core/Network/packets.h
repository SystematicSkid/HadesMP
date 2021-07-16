#pragma once
#define ENET_IMPLEMENTATION

#include <enetpp/include/server.h>
#include <enetpp/include/client.h>

#include <msgpack/msgpack.hpp>

namespace core::network::packets
{
	enum class PacketType : uint8_t
	{
		OnConnected = 0,
		OnHandshake,
		OnDiconnected,
		OnUpdate,
		OnPlayerInput,
		OnPlayerLoaded,
		OnMovementUpdate,
		OnAttack,
		OnAddEnemy
	};

	/* Universal packet header, used by every packet */
	class Packet
	{
	public:
		Packet(PacketType type, msgpack::object obj) : type((uint8_t)type), data(obj) {}
		Packet() : type(0) {}
	public:
		uint8_t type;
		msgpack::object data;
		MSGPACK_DEFINE(type, data);
	};

	class UpdatePacket
	{
	public:
		int tick;

		MSGPACK_DEFINE(tick);
	};

	class PlayerInputPacket
	{
	public:
		PlayerInputPacket() {}
		PlayerInputPacket(engine::hades::InputHandler* input)
		{
			this->buttons = input->GetButtons();
		}
	public:

		std::vector<int> buttons;

		MSGPACK_DEFINE(buttons);
	};

	class MovementPacket 
	{
	public:
		MovementPacket() {}
		MovementPacket(engine::hades::Unit* unit)
		{
			this->location_x = unit->location.x;
			this->location_y = unit->location.y;
			this->velocity_x = 0.f;
			this->velocity_y = 0.f;
			this->angle		 = unit->pBody->mAngle;
			this->is_moving	 = unit->mIsMoving;
		}
	public:
		float	location_x;
		float	location_y;
		float	velocity_x;
		float	velocity_y;
		float	angle;
		bool	is_moving;

		MSGPACK_DEFINE(location_x, location_y, velocity_x, velocity_y, angle, is_moving);
	};

	class AttackPacket
	{
	public:
		AttackPacket() {}
		AttackPacket(engine::hades::Weapon* weapon, float angle, D3DXVECTOR2 target_position, engine::hades::Thing* target, bool apply_self, D3DXVECTOR2 from)
		{
			weapon_hash = weapon->pData->name.ToString();
			this->angle = angle;
			target_position_x = target_position.x;
			target_position_y = target_position.y;
			if(target)
			{
				engine::misc::HashGuid tmp;
				auto name = target->GetName(&tmp);
				this->target_name = name->ToString();
			}
			else
			{
				this->target_name = "";
			}

			this->from_position_x = from.x;
			this->from_position_y = from.y;
			this->apply_self_effects = apply_self;

			/* Set weapon data */
			reload_paused = weapon->mReloadPaused;
			require_control_release = weapon->mRequireControlRelease;
			charging = weapon->mCharging;
			last_fire_time = weapon->mLastFireTime;
			control_time = weapon->mControlTime;
			cooldown_time_remaining = weapon->mCooldownTimeRemaining;
			ammo = weapon->mAmmo;
			volley = weapon->mVolley;
			reload_time_remaining = weapon->mReloadTimeRemaining;
		}

	public:
		std::string weapon_hash;
		std::string target_name;
		float angle;
		float target_position_x;
		float target_position_y;
		float from_position_x;
		float from_position_y;
		bool apply_self_effects;
		/* Weapon data */
		bool reload_paused;
		bool require_control_release;
		bool charging;
		float last_fire_time;
		float control_time;
		float cooldown_time_remaining;
		int ammo;
		int volley;
		float reload_time_remaining;


		MSGPACK_DEFINE(weapon_hash, angle, target_position_x, target_position_y, reload_paused, 
			require_control_release, charging, last_fire_time, control_time, cooldown_time_remaining, ammo, volley, reload_time_remaining);
	};

	class WeaponUpdatePacket
	{
	public:
		WeaponUpdatePacket() {}

	public:

		std::string weapon_hash;
		/* Weapon data */
		bool reload_paused;
		bool require_control_release;
		bool charging;
		float last_fire_time;
		float control_time;
		float cooldown_time_remaining;
		int ammo;
		int volley;
		float reload_time_remaining;
	};

	class AddEnemyPacket
	{
	public:
		AddEnemyPacket() {}
		AddEnemyPacket(engine::hades::Unit* unit, D3DXVECTOR2 location)
		{
			this->enemy_name = unit->pData->name.ToString();
			this->id = unit->mId;
			this->location_x = location.x;
			this->location_y = location.y;
		}
	public:
		std::string enemy_name;
		int id;
		float location_x;
		float location_y;

		MSGPACK_DEFINE(enemy_name, id, location_x, location_y);
	};
}