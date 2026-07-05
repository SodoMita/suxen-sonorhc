import os
import time
from chrono_agent import ChronoAgent


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
