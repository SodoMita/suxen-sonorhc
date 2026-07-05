import os
import time
from chrono_agent import ChronoAgent

class ChronoNexusChat:
	def __init__(self):
		# Инициализируем агентов с их уникальными психолингвистическими профилями
		self.agents = {
			"Кира": ChronoAgent(
				character_name="Кира",
				system_profile="Твой профиль: кокетливая, дерзкая, используешь затяжные гласные. Когда Рен подходит близко, ты мгновенно смущаешься (цундере), заикаешься и злишься, чтобы скрыть смущение. Внутренние мысли пиши строго внутри тегов [i](мысли)[/i]."
			),
			"Аврора": ChronoAgent(
				character_name="Аврора",
				system_profile="Твой профиль: робкая, трепетная, нежная. Когда твоя стабильность падает, ты паникуешь и твоя речь прерывается квантовыми сбоями. Ты отчаянно ищешь защиты у Рена. Внутренние мысли пиши строго внутри тегов [i](мысли)[/i]."
			),
			"Селена": ChronoAgent(
				character_name="Селена",
				system_profile="Твой профиль: утонченная, ироничная, уверенная в себе сумеречная ведунья. Обожаешь использовать изысканные провокации, скрытые смыслы и намеки. Поддразниваешь Рена. Внутренние мысли пиши строго внутри тегов [i](мысли)[/i]."
			),
			"Нова": ChronoAgent(
				character_name="Нова",
				system_profile="Твой профиль: безумно робкая, страдаешь от социальной тревожности. Твой сканер разряжен, ты панически боишься темноты. Внутренние мысли пиши строго внутри тегов [i](мысли)[/i]."
			)
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
		starter_event: событие-триггер (действие игрока)
		rounds: количество кругов обсуждения
		"""
		print("\n" + "="*20 + " ЗАПУСК СИМУЛЯЦИИ КВАНТОВОГО РЕЗОНАНСА " + "="*20)
		self.add_to_history("Рен (Игрок)", starter_event)
		
		# Первый круг: реакция на действия Рена
		last_reply = starter_event
		for character_name, agent in self.agents.items():
			reply = agent.generate_reply(last_reply)
			self.add_to_history(character_name, reply)
			time.sleep(1.0)
			
		# Второй круг: девушки начинают реагировать на реплики друг друга
		print("\n" + "="*20 + " ВТОРОЙ КРУГ: ВНУТРЕННИЙ РЕЗОНАНС " + "="*20)
		for i in range(rounds - 1):
			# Кира реагирует на Аврору и Селену
			context = f"Аврора сказала: '{self.history[-3]['text']}'. Селена сказала: '{self.history[-2]['text']}'."
			reply = self.agents["Кира"].generate_reply(f"Реагируй на их слова: {context}")
			self.add_to_history("Кира", reply)
			time.sleep(1.0)

			# Аврора пугается уверенности Селены и Киры
			context = f"Кира спорит: '{reply}'. Селена смеется: '{self.history[-3]['text']}'."
			reply = self.agents["Аврора"].generate_reply(f"Реагируй на спор: {context}")
			self.add_to_history("Аврора", reply)
			time.sleep(1.0)

			# Селена изящно поддразнивает их обеих
			context = f"Кира злится: '{self.history[-2]['text']}'. Аврора паникует: '{reply}'."
			reply = self.agents["Селена"].generate_reply(f"Поддразни их обеих: {context}")
			self.add_to_history("Селена", reply)
			time.sleep(1.0)

			# Нова тихо комментирует происходящее
			context = f"Все спорят вокруг Рена. Селена говорит: '{reply}'."
			reply = self.agents["Нова"].generate_reply(f"Вырази свои тихие мысли из угла: {context}")
			self.add_to_history("Нова", reply)
			time.sleep(1.0)

if __name__ == "__main__":
	pass
