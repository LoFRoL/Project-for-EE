#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <ctime>


//#define DEBUG	// Debug output
//#define ANYKEY	// Break on every stage

int GetRandomNum(int max) {
	int NewRand;
	if (max == 0) return 0;
	else NewRand = rand() % max;
	return NewRand;
}

class Worker {
public:
	Worker() {
		Deleted = false;
		Active = true;
		MySubscribe = {};
		MySubscribeCount = {};
		MySubscribeEvent = {};
		MySubscribeSum = {};
		TransferNumber = 0;
	}

	void Subscribe(Worker GlobalMassive[], int OurIndex, size_t Size) {
		std::vector<int> PossibleSub = {};
		for (size_t i = 0; i < MySubscribe.size(); ++i) { // Check ourSubs
			for (size_t j = 0; j < GlobalMassive[MySubscribe[i]].MySubscribe.size(); ++j) {
				int CurrentIndex = GlobalMassive[MySubscribe[i]].MySubscribe[j];
				if (CurrentIndex != OurIndex) {
					bool GoodOne = true;
					for (size_t k = 0; k < MySubscribe.size(); ++k) {
						if (CurrentIndex == MySubscribe[k]) GoodOne = false;
					}
					if (GoodOne) {
						for (size_t k = 0; k < PossibleSub.size(); ++k) {
							if (CurrentIndex == PossibleSub[k]) GoodOne = false;
						}
						if (GoodOne) PossibleSub.push_back(CurrentIndex);
					}
				}
			}
		}
		for (size_t i = 0; i < Size; ++i) { // Check who have our in sabs
			if (i != OurIndex) {
				for (size_t j = 0; j < GlobalMassive[i].MySubscribe.size(); ++j) {
					if (GlobalMassive[i].MySubscribe[j] = OurIndex) {
						bool GoodOne = true;
						for (size_t k = 0; k < MySubscribe.size(); ++k) {
							if (i == MySubscribe[k]) GoodOne = false;
						}
						if (GoodOne) {
							for (size_t k = 0; k < PossibleSub.size(); ++k) {
								if (i == PossibleSub[k]) GoodOne = false;
							}
							if (GoodOne) PossibleSub.push_back(i);
						}
					}
				}
			}
		}
		if (PossibleSub.size() != 0) {
			MySubscribe.push_back(PossibleSub[GetRandomNum(PossibleSub.size())]);
			MySubscribeSum.push_back(0);
			MySubscribeCount.push_back(0);
			MySubscribeEvent.push_back(nullptr);
			SetOutputTransferEvent(MySubscribeEvent.size()-1);
		}

#if defined DEBUG
		if (!MySubscribe.empty()) std::cout << "DEBUG: " << OurIndex << " has subscribe to " << MySubscribe.back() << std::endl;
#endif
	}

	void UnSubscribe() {
		if (!MySubscribe.empty()) {
			int RandIndex = GetRandomNum(MySubscribe.size());

			if (!MySubscribe.empty())		MySubscribe.erase(MySubscribe.begin() + RandIndex);
			if (!MySubscribeSum.empty())	MySubscribeSum.erase(MySubscribeSum.begin() + RandIndex);
			if (!MySubscribeCount.empty())	MySubscribeCount.erase(MySubscribeCount.begin() + RandIndex);
			if (!MySubscribeEvent.empty())	MySubscribeEvent.erase(MySubscribeEvent.begin() + RandIndex);
		}
	}

	bool FindVictim(size_t victim) {
		for (size_t i = 0; i < MySubscribe.size(); ++i) {
			if (MySubscribe[i] == victim) {

#if defined DEBUG
				std::cout << "DEBUG(CheckConnection): has subscribe " << victim << std::endl;
#endif

				return true;
			}
		}
		return false;
	}

	void ClearData() {
		MySubscribe.clear();
		MySubscribeCount.clear();
		MySubscribeSum.clear();
		MySubscribeEvent.clear();
		TransferNumber = 0;
		Deleted = true;
		Active = false;
	}

	void GenerateNewTransfer() {
		TransferNumber = GetRandomNum(60) + 1; // 60 is for example, it can be any number // +1 for generate number, which != 0
	}

	void SetHolded() {
		Deleted = false;
	}

	void SetOutputTransferEvent(int index) { // Set Output Options
		int RandNumber = GetRandomNum(2);
		if (RandNumber == 0) {
			MySubscribeEvent[index] = &Worker::OutputTransferEvent1;
		}
		else {
			MySubscribeEvent[index] = &Worker::OutputTransferEvent2;
		}
	}

	void TransferEvent(Worker GlobalMassive[], size_t OurIndex) {
		for (size_t i = 0; i < MySubscribe.size(); ++i) {
			if (MySubscribeEvent[i] != nullptr) {
				int Transfer = GlobalMassive[MySubscribe[i]].GetTransfer();
				if (Transfer > 0) {
					MySubscribeSum[i] += Transfer;
					++MySubscribeCount[i];
					(this->*MySubscribeEvent[i])(OurIndex, i, MySubscribeSum[i], MySubscribeCount[i]);
				}
			}
		}
	}

	int GetTransfer() {
		return TransferNumber;
	}

	bool GetActive() {
		return Active;
	}

	bool GetDeleted() {
		return Deleted;
	}
	
	std::vector<int> MySubscribe;		// Massive of his subscriptions
	std::vector<int> MySubscribeSum;	// Massive of sum, which was getted from his subscriptions
	std::vector<int> MySubscribeCount;	// Massive of count operations, which was getted from his subscriptions
	std::vector<void (Worker::*)(int FirstIndex, int SecondIndex, int Sum, int Count)> MySubscribeEvent;	//Massive of ptr = Output transfer event
	// Можно было реализовать через булевый массив, но посчитал что так будет красивее
private:
	// Options of output of Transfer event
	void OutputTransferEvent1(int FirstIndex, int SecondIndex, int Sum, int Count) {
		std::cout << FirstIndex << " -> " << SecondIndex << " : S = " << Sum << std::endl;
	}
	void OutputTransferEvent2(int FirstIndex, int SecondIndex, int Sum, int Count) {
		std::cout << FirstIndex << " -> " << SecondIndex << " : N = " << Count << std::endl;
	}
private:
	// Variables
	int TransferNumber;					// Number which worker will give for his subscribes
	bool Active;						// If false: simulate will skip this object
	bool Deleted;						// If true: simulate will replace this object for new one
};








int main() {
	srand(static_cast<unsigned int>(time(NULL)));
	std::vector<Worker> Workers;			// Massive with object of Workers
	int StartSize = rand() % 10;			// Start size of Workers. Random 0-9
	for (int i = 0; i < StartSize; ++i) {
		Workers.emplace_back(Worker());
	}

#if defined DEBUG
	std::cout << "Workers are filled\n";
	std::cout << "Workers.size() = " << Workers.size() << std::endl;;
	std::cout << "StartSize = " << StartSize << std::endl;;
#endif

	for (size_t i = 0; i < Workers.size(); ++i) {
		// First subscribe
		bool GoodSub = true;
		int RandSubIndex = rand() % Workers.size();
		if (Workers.size() < 2) break;
		else if (RandSubIndex != i) Workers[i].MySubscribe.push_back(RandSubIndex);
		else if (i != Workers.size() - 1) Workers[i].MySubscribe.push_back(RandSubIndex + 1);
		else if (i != 0) Workers[i].MySubscribe.push_back(RandSubIndex - 1);
		else {
			--i;
			GoodSub = false;
		}
		if (GoodSub) {
			Workers[i].MySubscribeSum.push_back(0);
			Workers[i].MySubscribeCount.push_back(0);
			Workers[i].MySubscribeEvent.push_back(0);
			Workers[i].SetOutputTransferEvent(0);		// Generate transfer event for 1 sub
		}
	}

#if defined DEBUG
	std::cout << "Workers getted first subs\n";
#endif

	char answer = 'n';
	int SubChance = 1;				// Chance for Subscribe
	int UnsubChance = 1;			// Chance for Unsubscribe
	int SkipChance = 1;				// Chance for Skip
	int CreateNumChance = 1;		// Chance for Create new Transfer Number
	int CreateNewWorkerChance = 1;	// All chances
	std::cout << "Would you like to costumize chances of that simulation? (y or anykey): ";
	std::cin >> answer;
	if (answer == 'y') {
		std::cout << "Enter subscribe chance(0-100): ";
		std::cin >> SubChance;
		std::cout << "Enter unsubscribe chance(0-100): ";
		std::cin >> UnsubChance;
		std::cout << "Enter skip process chance(0-100): ";
		std::cin >> SkipChance;
		std::cout << "Enter chance of create new worker and subscribe it(0-100): ";
		std::cin >> CreateNewWorkerChance;
		std::cout << "Enter chance of create number(0-100): ";
		std::cin >> CreateNumChance;
	}
	/// 
	////
	///// Start Simulate
	////
	/// 
	bool OneWorkerAlive = true;
	for (int Stage = 0; OneWorkerAlive; ++Stage) { 
		std::cout << "\n\n" << Stage << " - stage of simulate\n";

#if defined DEBUG
		std::cout << "DEBUG: (CONTROL SIZE OF WORKERS): \n";
		for (size_t i = 0; i < Workers.size(); ++i) {
			std::cout << "Workers[" << i << "].MySubs.size() = " << Workers[i].MySubscribe.size() << std::endl;
			std::cout << "Workers[" << i << "].MySubs = [ ";	for (size_t j = 0; j < Workers[i].MySubscribe.size(); ++j) { std::cout << Workers[i].MySubscribe[j] << " "; }	std::cout << "]\n";
			std::cout << "Workers[" << i << "].Active = ";	Workers[i].GetActive() ? std::cout << "Yes\n" : std::cout << "No\n";
			std::cout << "Workers[" << i << "].IsDead = ";	Workers[i].GetDeleted() ? std::cout << "Yes\n" : std::cout << "No\n";
			std::cout << "Workers[" << i << "].TransferNum = " << Workers[i].GetTransfer() << std::endl;
			std::cout << "Workers[" << i << "].MySubsSum.size() = " << Workers[i].MySubscribeSum.size() << std::endl;
			std::cout << "Workers[" << i << "].MySubsSums = [ ";	for (size_t j = 0; j < Workers[i].MySubscribeSum.size(); ++j) { std::cout << Workers[i].MySubscribeSum[j] << " "; }	std::cout << "]\n";
			std::cout << "Workers[" << i << "].MySubsCount.size() = " << Workers[i].MySubscribeCount.size() << std::endl;
			std::cout << "Workers[" << i << "].MySubsCounts = [ ";	for (size_t j = 0; j < Workers[i].MySubscribeCount.size(); ++j) { std::cout << Workers[i].MySubscribeCount[j] << " "; }	std::cout << "]\n";
			std::cout << "Workers[" << i << "].MySubsevent.size() = " << Workers[i].MySubscribeEvent.size() << std::endl;
			std::cout << "Workers[" << i << "].MySubsEvents = [ ";	for (size_t j = 0; j < Workers[i].MySubscribeEvent.size(); ++j) { std::cout << Workers[i].MySubscribeEvent[j] << " "; }	std::cout << "]\n\n";
		}
		std::cout << "============================================================================\n";
#endif

		// Subscribers get transfer
		for (size_t i = 0; i < Workers.size(); ++i) {
			Workers[i].TransferEvent(&Workers[0], i);
		}

		// Create massive for new subs (Call in th end of stage)
		std::map<int, int> NewSubscribers; // All new subscribers: First=our index, Second=sub index

		size_t lastWorkerSize = Workers.size(); // lock size of workers for this stage
		// Generate event
		for (size_t i = 0; i < lastWorkerSize; ++i) {
			int RandEvent = rand() % (CreateNumChance + SubChance + UnsubChance + SkipChance + CreateNewWorkerChance); // 0-500
			/* Рандом работает по частям, указанные переменные отвечающие за вероятность какого либо действия имеют часть  пропорциональному этому числу
			* К примеру: Шанс на подписку(SubChance) = 5, а сумма всех шансов равна 36
			* Тогда шанс подписки(SubChance) = 5/36
			* 
			* Имея рандомное число мы просто идем по диапозонам ивентов, пример:
			* CreateNumChance = 5, SubChance = 7, UnsubChance = 2, SkipChance = 9, CreateNewWorkerChance = 1
			* Сумма = 24, рандомное число = rand() % 24, к примеру = 8
			* Тогда диапозоны ивентов будут равны:
			* CreateNum [0:5), Sub [5:12), UnSub [12:14), CreateNewWorker [14:15), SkipChance [15:24) */
			if (Workers[i].GetActive() && !Workers[i].GetDeleted()) {
				if (RandEvent >= 0 && RandEvent < CreateNumChance) {
					// CreateNum
					Workers[i].GenerateNewTransfer();

#if defined DEBUG
					std::cout << "DEBUG: EVENT(CreateNum) : index = " << i << " new TrasferNum = " << Workers[i].GetTransfer() << std::endl;
#endif

				}
				else if (RandEvent >= CreateNumChance && RandEvent < SubChance + CreateNumChance) {
					// Sub
					int beginSubs = Workers[i].MySubscribe.size();
					Workers[i].Subscribe(&Workers[0], i, lastWorkerSize);
					// if subscribe is success, erase this sub for this stage and add it on the end of stage
					if (!Workers[i].MySubscribe.empty() && beginSubs != Workers[i].MySubscribe.size()) { 
						NewSubscribers.insert(std::pair<int, int>(i, Workers[i].MySubscribe.back()));
						Workers[i].MySubscribe.erase(Workers[i].MySubscribe.end() - 1);
						Workers[i].MySubscribeSum.erase(Workers[i].MySubscribeSum.end() - 1);
						Workers[i].MySubscribeEvent.erase(Workers[i].MySubscribeEvent.end() - 1);
						Workers[i].MySubscribeCount.erase(Workers[i].MySubscribeCount.end() - 1);

					}
					else {
						// Skip Sub

#if defined DEBUG
						std::cout << "DEBUG: EVENT(Sub) : index = " << i << " Skipped " << std::endl;
#endif
					}


				}
				else if (RandEvent >= SubChance + CreateNumChance && RandEvent < UnsubChance + SubChance + CreateNumChance) {
					// Unsub
					Workers[i].UnSubscribe();

#if defined DEBUG
					std::cout << "DEBUG: EVENT(Unsubsribe) : index = " << i << " new subscribe size = " << Workers[i].MySubscribe.size() << std::endl;
#endif

				}
				else if (RandEvent >= UnsubChance + SubChance + CreateNumChance && RandEvent < CreateNewWorkerChance + UnsubChance + SubChance + CreateNumChance) {
					// CreateNewWorker
					bool HaveClearSector = false; // Findded clear sector
					// Ищем пустой сектор для нового работника
					for (size_t h = 0; h < lastWorkerSize; ++h) { 
						// Если находим то занимаем его и запоминаем его для добавления в сабы в конце стэйджа
						if (Workers[h].GetDeleted()) {
							HaveClearSector = true;
							Workers[h].SetHolded();
							NewSubscribers.insert(std::pair<int, int>(i, h));
#if defined DEBUG
							std::cout << "DEBUG(CreateNewWorker): " << i << " finded clear sector " << h << std::endl;
#endif
						}
					}
					if (!HaveClearSector) {
						// Create new space for it
						Workers.emplace_back(Worker());
						// sub on it
						NewSubscribers.insert(std::pair<int, int>(i, Workers.size() - 1));

#if defined DEBUG
						std::cout << "DEBUG(CreateNewWorker): " << i << " not finded clear sector " << std::endl;
#endif
					}

#if defined DEBUG
					std::cout << "DEBUG: EVENT(CreateNewWorker) : index = " << i << " new subscribe  = " << Workers.size() - 1 << std::endl;
#endif

				}
				else {
					// Skip

#if defined DEBUG
					std::cout << "DEBUG: EVENT(Skip) : index = " << i << std::endl;
#endif

				}
			}
		}

		// Add all new subscribers
#if defined DEBUG
		for (auto it = NewSubscribers.begin(); it != NewSubscribers.end(); ++it) {
			std::cout << "DEBUG: EVENT(SUBS ADD) : " << it->first << " " << it->second << std::endl;
		}
#endif

		for (auto it = NewSubscribers.begin(); it != NewSubscribers.end(); ++it) {
			Workers[it->first].MySubscribe.push_back(it->second);
			Workers[it->first].MySubscribeCount.push_back(0);
			Workers[it->first].MySubscribeSum.push_back(0);
			Workers[it->first].MySubscribeEvent.push_back(nullptr);
			Workers[it->first].SetOutputTransferEvent(Workers[it->first].MySubscribeEvent.size()-1);
		}
		NewSubscribers.clear();

		// Clear workers with 0 subscribes
		for (size_t i = 0; i < lastWorkerSize; ++i) {
			if (Workers[i].MySubscribe.size() < 1 && Workers[i].GetActive() && !Workers[i].GetDeleted()) {
				bool IsDelete = true;
				for (size_t j = 0; j < Workers.size(); ++j) {
					if (Workers[j].FindVictim(i)) {
						IsDelete = false;
						break;
					}
				}
				if (IsDelete) {

#if defined DEBUG
					std::cout << "Worker " << i << " was deleted(0 connection)\n";
#endif

					Workers[i].ClearData();
					OneWorkerAlive = false;
				}
			}
		}

		// Add All new Workers
		for (size_t h = 0; h < Workers.size(); ++h) {
			if (!Workers[h].GetDeleted() && !Workers[h].GetActive()) { // if Deleted = false && Active = false
				// Set defaults settings of object in index - h
				Workers[h].Worker::Worker();


#if defined DEBUG
				std::cout << "Worker " << h << " now working!\n";
#endif
			}
		}


		if (Workers.size() == NULL) { // Cancel simulate if count of Workers will 0
			break;
#if defined DEBUG
			std::cout << "0 Workers!\n";
#endif
		}

		// Если удалялся работник то проверяем есть ли живые
		if (!OneWorkerAlive) {
			for (size_t h = 0; h < lastWorkerSize; ++h) {
				if (!Workers[h].GetDeleted()) {
					OneWorkerAlive = true;
#if defined DEBUG
					std::cout << "DEBUG: One or more workers are alive!\n";
#endif
					break;
				}
			}
		}

#if defined ANYKEY
		std::cout << "Press any char to continue ... : ";
		std::cin >> answer;
#endif
	}
	std::cout << "\nSimulation is over :(\n";
}
