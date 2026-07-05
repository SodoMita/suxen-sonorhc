import os
import time
import json
from chrono_agent import ChronoAgent

class ChronoNexusEngine:
	def __init__(self):
		# База данных локаций с их физическими параметрами
		self.locations = {
			"japanese_classroom": {
				"desc": "Аварийный отсек японской школы. Гравитация нестабильна, парты парят, за окном бушует квантовый шторм Юпитера.",
				"bg_path": "res://bgs/japanese_school_class.jpg"
			},
			"lab_station": {
				"desc": "Жилой блок станции Прометей. Скорость света снижена, легкое визуальное размытие, воздух пахнет озоном.",
				"bg_path": "res://bgs/lab_station.jpg"
			},
			"starry_sanctum": {
				"desc": "Лунное святилище. Гравитация 0.15g, парящие белые колонны. Ткани одежды теряют плотность и скользят.",
				"bg_path": "res://bgs/sanctum.jpg"
			},
			"twilight_archives": {
				"desc": "Сумеречные архивы. Время течет петлями, сиреневый туман, парящие полуразрушенные свитки.",
				"bg_path": "res://bgs/cyberpunk_alley.jpg"
			},
			"crystal_core": {
				"desc": "Кристальный реакторный кор. Полная физическая нестабильность, постоянные перегрузки, фазовые сдвиги материи.",
				"bg_path": "res://bgs/core.jpg"
			}
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
			"Nova": ChronoAgent(
				character_name="Nova",
				public_profile="Твой профиль: безумно робкая исследовательница, страдаешь от социальной тревожности. Боишься темноты.",
				private_secrets="Твой сканер разряжен, и только тень Рена спасает тебя от панических атак в темноте."
			)
		}

	def run_scene_segment(self, location_id, public_history, last_action, responding_chars):
		location_data = self.locations.get(location_id, {"desc": "Неизвестная квантовая зона", "bg_path": "res://bgs/japanese_school_class.jpg"})
		location_context = location_data["desc"]
		
		segment_log = []
		local_history = list(public_history)
		local_history.append(f"Рен (Игрок): {last_action}")

		for name in responding_chars:
			if name not in self.agents:
				continue
			agent = self.agents[name]
			
			private_thought = agent.generate_hidden_thought(local_history, location_context)
			public_reply = agent.generate_public_dialogue(local_history, private_thought, location_context)
			
			segment_log.append({
				"character": name,
				"dialogue": public_reply
			})
			local_history.append(f"{name}: {public_reply}")
			time.sleep(0.5)
			
		return segment_log

	def evaluate_branching_path(self, location_id, choices_dict):
		"""
		Создает развилку, проходит поочередно каждую ветку, сохраняет результаты в JSON
		и генерирует совместимый .dtl файл для Dialogic 2.
		"""
		print("\n" + "="*20 + " ОБНАРУЖЕНА КВАНТОВАЯ РАЗВИЛКА " + "="*20)
		
		branch_results = {}
		for choice_name, choice_data in choices_dict.items():
			print(f"\n>>>> [ПРОХОЖДЕНИЕ ВЕТВИ: {choice_name.upper()}] <<<<")
			
			segment_log = self.run_scene_segment(
				location_id=location_id,
				public_history=[],
				last_action=choice_data["action"],
				responding_chars=choice_data["chars"]
			)
			branch_results[choice_name] = {
				"action": choice_data["action"],
				"dialogue_steps": segment_log
			}

		# 1. Сохраняем чистый лог симуляции в JSON
		output_json_path = "docs/vn_specific/simulated_branches.json"
		os.makedirs(os.path.dirname(output_json_path), exist_ok=True)
		with open(output_json_path, "w", encoding="utf-8") as f:
			json.dump(branch_results, f, ensure_ascii=False, indent=4)
		print(f"\n[Система]: Лог симуляции успешно сохранен в JSON: {output_json_path}")

		# 2. Конвертируем результаты в Dialogic 2 .dtl формат
		self.convert_json_to_dialogic_timeline(output_json_path, location_id)

		print("\n" + "="*20 + " ВСЕ ВЕТВИ РАЗВИЛКИ УСПЕШНО СИМУЛИРОВАНЫ " + "="*20)
		return branch_results

	def convert_json_to_dialogic_timeline(self, json_path, location_id):
		"""
		Конвертирует JSON-лог симуляции в чистый, рабочий .dtl файл Dialogic 2
		и сохраняет его в папку timelines/.
		"""
		with open(json_path, "r", encoding="utf-8") as f:
			data = json.load(f)

		location_data = self.locations.get(location_id, {"bg_path": "res://bgs/japanese_school_class.jpg"})
		bg_path = location_data["bg_path"]

		dtl_lines = []
		# Задаем фоновое окружение
		dtl_lines.append(f'[background arg="{bg_path}" fade="1.2"]')
		dtl_lines.append("join player center [animation=\"Bounce In\"]")
		dtl_lines.append("player: Что же мне сделать дальше? Квантовые флуктуации вокруг нарастают...")

		# Сценарий выбора игрока
		for choice_name, choice_data in data.items():
			dtl_lines.append(f"- {choice_name}")
			
			# Наводим фокус на ветвь. Действие Рена (Игрока) пишем с мыслями
			action = choice_data["action"]
			dtl_lines.append(f'\tplayer: ({action})')

			# Рендерим диалоги персонажей
			for step in choice_data["dialogue_steps"]:
				char_name = step["character"]
				dialogue_text = step["dialogue"]
				
				# Приводим имя персонажа к лоуэркейсу для dch
				char_ref = char_name.lower()
				
				# Исключаем теги мыслей [i] [/i] из реплик персонажей (мысли персонажей убираем из dtl)
				clean_dialogue = dialogue_text
				if "[i]" in dialogue_text and "[/i]" in dialogue_text:
					# Убираем все, что внутри [i] и [/i]
					start_idx = dialogue_text.find("[i]")
					end_idx = dialogue_text.find("[/i]") + 4
					clean_dialogue = dialogue_text[:start_idx] + dialogue_text[end_idx:]
					clean_dialogue = clean_dialogue.strip()

				# Сдвиг таба на всю глубину ветвления
				dtl_lines.append(f'\tjoin {char_ref} right [animation="Slide From Right"]')
				
				# Эскейпим возможные двоеточия
				safe_dialogue = clean_dialogue.replace(":", "\\:")
				dtl_lines.append(f'\t{char_ref}: {safe_dialogue}')
				dtl_lines.append(f'\tleave {char_ref} [animation="Fade Out Down"]')

		# Сохраняем в timelines/
		output_dtl_path = "timelines/TL_006_Simulated_Branch.dtl"
		os.makedirs(os.path.dirname(output_dtl_path), exist_ok=True)
		with open(output_dtl_path, "w", encoding="utf-8") as f:
			f.write("\n".join(dtl_lines))
		print(f"[Система]: Dialogic 2 Timeline успешно сгенерирован: {output_dtl_path}")

if __name__ == "__main__":
	# Инструкция по локальному запуску:
	# 1. Задайте переменные окружения в вашей ОС:
	#    export YANDEX_API_KEY="ваш_ключ_api"
	#    export YANDEX_MODEL_URI="ваш url yandex ai studio"
	# 2. Запустите скрипт: python3 chrono_chat.py
	pass
