#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// 1. PotionRecipe 클래스 (데이터 모델)

class PotionRecipe {
private:
    string name;
    vector<string> ingredients;

public:
    PotionRecipe(const string& n, const vector<string>& ing) : name(n), ingredients(ing) {}

    string GetName() const { return name; }
    const vector<string>& GetIngredients() const { return ingredients; }
};

// 2. RecipeManager 클래스 (레시피 관리 전담)

class RecipeManager {
private:
    vector<PotionRecipe> recipes;

public:
    void AddRecipe(const string& name, const vector<string>& ingredients) {
        recipes.emplace_back(name, ingredients);
    }

    // 이름으로 검색 (필수 기능)
    PotionRecipe* FindByName(const string& name) {
        for (auto& r : recipes) {
            if (r.GetName() == name) return &r;
        }
        return nullptr;
    }

    // 재료로 검색 (필수 기능)
    vector<PotionRecipe*> FindByIngredient(const string& ingredient) {
        vector<PotionRecipe*> results;
        for (auto& r : recipes) {
            for (const auto& ing : r.GetIngredients()) {
                if (ing == ingredient) {
                    results.push_back(&r);
                    break;
                }
            }
        }
        return results;
    }

    const vector<PotionRecipe>& GetAll() const { return recipes; }
};

// 3. StockManager 클래스 (재고 관리 전담)
class StockManager {
private:
    map<string, int> stocks;
    const int MAX_STOCK = 3;

public:
    void InitializeStock(const string& name) {
        stocks[name] = MAX_STOCK; // 새 레시피 추가 시 3개 기본 지급
    }

    bool Dispense(const string& name) {
        if (stocks.count(name) && stocks[name] > 0) {
            stocks[name]--;
            return true;
        }
        return false;
    }

    void ReturnBottle(const string& name) {
        if (stocks.count(name)) {
            if (stocks[name] < MAX_STOCK) {
                stocks[name]++;
                cout << ">> '" << name << "' 공병이 반환되어 재고가 늘었습니다. (현재: " << stocks[name] << ")" << endl;
            }
            else {
                cout << ">> '" << name << "' 재고가 이미 가득 찼습니다. (최대: " << MAX_STOCK << ")" << endl;
            }
        }
    }

    int GetStock(const string& name) const {
        return stocks.count(name) ? stocks.at(name) : 0;
    }
};

// 4. AlchemyWorkshop 클래스 (최종 시스템 인터페이스)

class AlchemyWorkshop {
private:
    RecipeManager recipeManager;
    StockManager stockManager;

public:
    void RegisterRecipe(const string& name, const vector<string>& ingredients) {
        recipeManager.AddRecipe(name, ingredients);
        stockManager.InitializeStock(name);
        cout << ">> 레시피 등록 및 초기 재고(3개)가 설정되었습니다." << endl;
    }

    void SearchAndDispense(const string& query) {
        // 1. 이름으로 먼저 검색
        PotionRecipe* byName = recipeManager.FindByName(query);
        if (byName) {
            TryDispense(byName->GetName());
            return;
        }

        // 2. 이름에 없으면 재료로 검색
        vector<PotionRecipe*> byIng = recipeManager.FindByIngredient(query);
        if (!byIng.empty()) {
            cout << ">> 재료 '" << query << "'(이)가 포함된 물약들입니다:" << endl;
            for (auto* r : byIng) {
                cout << "   - " << r->GetName() << " ";
                TryDispense(r->GetName());
            }
        }
        else {
            cout << ">> '" << query << "'와 관련된 레시피를 찾을 수 없습니다." << endl;
        }
    }

    void TryDispense(const string& name) {
        if (stockManager.Dispense(name)) {
            cout << "[" << name << "] 지급 완료! (남은 재고: " << stockManager.GetStock(name) << ")" << endl;
        }
        else {
            cout << "[" << name << "] 재고가 부족하여 지급할 수 없습니다." << endl;
        }
    }

    void ReturnPotion(const string& name) {
        if (recipeManager.FindByName(name)) {
            stockManager.ReturnBottle(name);
        }
        else {
            cout << ">> 존재하지 않는 물약 이름입니다." << endl;
        }
    }

    void DisplayAll() {
        auto all = recipeManager.GetAll();
        if (all.empty()) { cout << "등록된 레시피가 없습니다.\n"; return; }

        cout << "\n--- [ 창고 현황 ] ---" << endl;
        for (const auto& r : all) {
            cout << "- " << r.GetName() << " [재고: " << stockManager.GetStock(r.GetName()) << "]" << endl;
            cout << "  재료: ";
            for (const auto& ing : r.GetIngredients()) cout << ing << " ";
            cout << endl;
        }
        cout << "---------------------\n";
    }
};

int main() {
    AlchemyWorkshop workshop;
    int choice;

    while (true) {
        cout << "\n 연금술 공방 관리 시스템\n1.레시피 추가 2.검색 및 지급 3.공병 반환 4.전체 목록 5.종료\n선택: ";
        cin >> choice;

        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        if (choice == 1) {
            string name;
            cout << "물약 이름: "; cin.ignore(); getline(cin, name);
            vector<string> ingredients;
            string ing;
            cout << "재료 입력 ('끝' 입력 시 종료): " << endl;
            while (getline(cin, ing) && ing != "끝") { ingredients.push_back(ing); }
            workshop.RegisterRecipe(name, ingredients);
        }
        else if (choice == 2) {
            string query;
            cout << "물약 이름 또는 재료 입력: "; cin.ignore(); getline(cin, query);
            workshop.SearchAndDispense(query);
        }
        else if (choice == 3) {
            string name;
            cout << "반환할 물약 이름: "; cin.ignore(); getline(cin, name);
            workshop.ReturnPotion(name);
        }
        else if (choice == 4) workshop.DisplayAll();
        else if (choice == 5) break;
    }
    return 0;
}