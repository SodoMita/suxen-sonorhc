import os
import time
from chrono_agent import ChronoAgent

<<<<<<< HEAD

class ChronoNexusChat:
    def __init__(self):
        # Инициализируем агентов с их уникальными психолингвистическими профилями
        self.agents = {
            "Кира": ChronoAgent(
                character_name="Кира",
                system_profile="Твой профиль: кокетливая, дерзкая, используешь затяжные гласные. Когда Рен подходит близко, ты смущаешься, заикаешься и злишься (цундере).",
            ),
            "Аврора": ChronoAgent(
                character_name="Аврора",
                system_profile="Твой профиль: робкая, трепетная, нежная. Когда твоя стабильность падает, ты паникуешь и твоя речь прерывается квантовыми сбоями. Ты отчаянно ищешь защиты у Рена.",
            ),
            "Селена": ChronoAgent(
                character_name="Селена",
                system_profile="Твой профиль: утонченная, ироничная, уверенная в себе сумеречная ведунья. Обожаешь использовать изысканные провокации, скрытые смыслы и намеки. Поддразниваешь Рена.",
            ),
            "Нова": ChronoAgent(
                character_name="Нова",
                system_profile="Твой профиль: безумно робкая, страдаешь от социальной тревожности. Твой сканер разряжен, ты панически боишься темноты. Выражаешь смущение тихим шепотом в скобках (...).",
            ),
        }

        # Очередь реплик для лога чата
        self.history = []

    def add_to_history(self, speaker, text):
        self.history.append({"speaker": speaker, "text": text})
        print(f"[{speaker}]: {text}")
        print("-" * 50)

    def run_group_discussion(self, starter_event, rounds=2):
        """
        Запуск симуляции группового диалога.
        starter_event: событие-триггер (например, действие Рена)
        rounds: количество кругов обсуждения между девушками
        """
        print("\n" + "=" * 20 + " ЗАПУСК СИМУЛЯЦИИ КВАНТОВОГО РЕЗОНАНСА " + "=" * 20)
        self.add_to_history("Рен (Силуэт)", starter_event)

        # Первый круг: реакция на действия Рена
        last_reply = starter_event
        for character_name, agent in self.agents.items():
            # Передаем реплику/действие Рена на вход каждому агенту
            reply = agent.generate_reply(last_reply)
            self.add_to_history(character_name, reply)
            # Даем небольшую паузу между запросами к API
            time.sleep(1.0)

        # Второй круг: девушки начинают реагировать на реплики друг друга
        print("\n" + "=" * 20 + " ВТОРОЙ КРУГ: ВНУТРЕННИЙ РЕЗОНАНС " + "=" * 20)
        for i in range(rounds - 1):
            # Кира реагирует на Аврору и Селену
            context = f"Аврора сказала: '{self.history[-3]['text']}'. Селена сказала: '{self.history[-2]['text']}'."
            reply = self.agents["Кира"].generate_reply(
                f"Реагируй на их слова: {context}"
            )
            self.add_to_history("Кира", reply)
            time.sleep(1.0)

            # Аврора пугается уверенности Селены и Киры
            context = (
                f"Кира спорит: '{reply}'. Селена смеется: '{self.history[-3]['text']}'."
            )
            reply = self.agents["Аврора"].generate_reply(f"Реагируй на спор: {context}")
            self.add_to_history("Аврора", reply)
            time.sleep(1.0)

            # Селена изящно поддразнивает их обеих
            context = f"Кира злится: '{self.history[-2]['text']}'. Аврора паникует: '{reply}'."
            reply = self.agents["Селена"].generate_reply(
                f"Поддразни их обеих: {context}"
            )
            self.add_to_history("Селена", reply)
            time.sleep(1.0)

            # Нова тихо шепчет из угла
            context = f"Все спорят вокруг Рена. Селена говорит: '{reply}'."
            reply = self.agents["Нова"].generate_reply(
                f"Вырази свои тихие мысли из угла: {context}"
            )
            self.add_to_history("Nova", reply)
            time.sleep(1.0)


if __name__ == "__main__":
    # Инструкция по локальному запуску:
    # 1. Задайте переменные окружения в вашей ОС:
    #    export YANDEX_API_KEY="ваш_ключ_api"
    #    export YANDEX_MODEL_URI="ваш url yandex ai studio"
    # 2. Запустите скрипт: python3 chrono_chat.py

    chat = ChronoNexusChat()

    # Сценарий: Рен делает выбор в пользу Авроры, сокращая дистанцию
    trigger = "Рен делает шаг вперед, берет Аврору за руку, чтобы стабилизировать ее мерцающий костюм, и мягко смотрит на остальных."
    chat.run_group_discussion(trigger, rounds=2)
=======
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
>>>>>>> main
