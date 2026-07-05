import os
import time
from chrono_agent import ChronoAgent

class ChronoNexusEngine:
	def __init__(self):
		# База данных локаций с их физическими параметрами
		self.locations = {
			"japanese_classroom": "Аварийный отсек японской школы. Гравитация нестабильна, парты парят, за окном бушует квантовый шторм Юпитера.",
			"lab_station": "Жилой блок станции Прометей. Скорость света снижена, легкое визуальное размытие, воздух пахнет озоном.",
			"starry_sanctum": "Лунное святилище. Гравитация 0.15g, парящие белые колонны. Ткани одежды теряют плотность и скользят.",
			"twilight_archives": "Сумеречные архивы. Время течет петлями, сиреневый туман, парящие полуразрушенные свитки.",
			"crystal_core": "Кристальный реакторный кор. Полная физическая нестабильность, постоянные перегрузки, фазовые сдвиги материи."
		}

		# База ИИ-агентов персонажей (весь каст)
		self.agents = {
			"Кира": ChronoAgent(
				character_name="Кира",
				public_profile="Твой профиль: кокетливая, дерзкая, используешь затяжные гласные. Когда Рен подходит близко, ты мгновенно смущаешься (цундере) и злишься.",
				private_secrets="Ты ревнуешь Рена ко всем остальным. Твои уши предательски дергаются, когда ты смотришь на него."
			),
			"Аврора": ChronoAgent(
				character_name="Аврора",
				public_profile="Твой профиль: робкая, трепетная, нежная. Когда твоя стабильность падает, ты паникуешь и твоя речь прерывается квантовыми сбоями.",
				private_secrets="Ты боишься полностью раствориться в пустоте ядра, и только присутствие Рена дает тебе физический вес."
			),
			"Селена": ChronoAgent(
				character_name="Селена",
				public_profile="Твой профиль: утонченная, ироничная сумеречная ведунья. Обожаешь использовать изысканные провокации и намеки.",
				private_secrets="Ты тайно изучаешь Рена как аномалию, но боишься, что его присутствие поглотит твою магию."
			),
			"Элара": ChronoAgent(
				character_name="Элара",
				public_profile="Твой профиль: кроткая, нежная целительница. Всегда вежлива и говорит нежными интонациями.",
				private_secrets="Ты разрывается между священным обетом чистоты и диким смущенным желанием обнять Рена."
			),
			"Ария": ChronoAgent(
				character_name="Ария",
				public_profile="Твой профиль: бывший Небесный Хранитель, говорящий с нежной грустью. Твой перья осыпаются искрами при сближении.",
				private_secrets="Ты готова полностью сжечь свои крылья и отказаться от божественности ради того, чтобы быть с Ренном."
			),
			"Нова": ChronoAgent(
				character_name="Нова",
				public_profile="Твой профиль: безумно робкая исследовательница, страдаешь от социальной тревожности. Боишься темноты.",
				private_secrets="Твой сканер разряжен, и только тень Рена спасает тебя от панических атак в темноте."
			)
		}

	def run_scene_segment(self, location_id, public_history, last_action, responding_chars):
		"""
		Прогон сегмента сцены на определенной локации.
		responding_chars: список персонажей, которые участвуют в этой сцене.
		"""
		location_context = self.locations.get(location_id, "Неизвестная квантовая зона")
		print(f"\n--- [Текущая Локация: {location_id.upper()}] ---")
		print(f"Физика окружения: {location_context}")
		print("-" * 60)

		# Сначала выводим действие игрока
		print(f"[Рен (Игрок)]: {last_action}")
		print("-" * 60)
		
		# Локальная история конкретного сегмента
		local_history = list(public_history)
		local_history.append(f"Рен (Игрок): {last_action}")

		# Каждый из выбранных персонажей генерирует мысли и реплику
		for name in responding_chars:
			if name not in self.agents:
				continue
			agent = self.agents[name]
			
			# Фаза 1: Генерация скрытой мысли с учетом локации
			private_thought = agent.generate_hidden_thought(local_history, location_context)
			print(f"  [Приватная Мысль {name} (Скрыто от других)]: {private_thought}")
			
			# Фаза 2: Генерация публичной реплики с учетом локации (глитчи одежды)
			public_reply = agent.generate_public_dialogue(local_history, private_thought, location_context)
			print(f"[{name}]: {public_reply}")
			print("-" * 60)
			
			# Добавляем в историю
			local_history.append(f"{name}: {public_reply}")
			time.sleep(1.0)
			
		return local_history

	def evaluate_branching_path(self, location_id, choices_dict):
		"""
		Создает развилку и ПООЧЕРЕДНО проходит каждый из вариантов выбора.
		choices_dict: словарь вида { \"Название выбора\": { \"action\": \"действие\", \"chars\": [список персонажей] } }
		"""
		print("\n" + "="*20 + " ОБНАРУЖЕНА КВАНТОВАЯ РАЗВИЛКА " + "="*20)
		
		branch_results = {}
		for choice_name, choice_data in choices_dict.items():
			print(f"\n>>>> [ПРОХОЖДЕНИЕ ВЕТВИ: {choice_name.upper()}] <<<<")
			
			# Запускаем сегмент сцены для данной ветви
			history = self.run_scene_segment(
				location_id=location_id,
				public_history=[],
				last_action=choice_data["action"],
				responding_chars=choice_data["chars"]
			)
			branch_results[choice_name] = history
			
		print("\n" + "="*20 + " ВСЕ ВЕТВИ РАЗВИЛКИ УСПЕШНО СИМУЛИРОВАНЫ " + "="*20)
		return branch_results

if __name__ == "__main__":
	# Инструкция по локальному запуску:
	# 1. Задайте переменные окружения в вашей ОС:
	#    export YANDEX_API_KEY="ваш_ключ_api"
	#    export YANDEX_MODEL_URI="ваш url yandex ai studio"
	# 2. Запустите скрипт: python3 chrono_chat.py
	pass
