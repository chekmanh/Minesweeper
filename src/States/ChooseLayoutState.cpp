#include "States/ChooseLayoutState.hpp"

#include "States/GamingState.hpp"
#include "Minesweeper/Field_content.hpp"
#include <filesystem>

ChooseLayoutState::ChooseLayoutState(State &oldState, Type type, int numPlayers)
    : State(oldState), type(type), numPlayers(numPlayers) {
  // std::cout << "init ChooseLayoutState with type: " << type << std::endl;
  this->initGui();
  if (type != BUILD) {
    this->gameType();
  }
}

ChooseLayoutState::~ChooseLayoutState() {
}

void ChooseLayoutState::gameType() {
  this->group = tgui::RadioButtonGroup::create();

  auto radio1 = tgui::RadioButton::create();
  radio1 = tgui::RadioButton::create();
  radio1->connect("checked", []() {
    Field_content::gameType = 2; // normal
  });
  this->group->add(radio1);

  auto radio2 = tgui::RadioButton::create();
  radio2->setPosition(tgui::bindLeft(radio1) + 120, tgui::bindTop(radio1));
  radio2->connect("checked", []() {
    Field_content::gameType = 0; // no rec
  });
  this->group->add(radio2);

  radio1 = tgui::RadioButton::create();
  radio1->setChecked(true);
  radio1->setPosition(tgui::bindLeft(radio2) + 120, tgui::bindTop(radio2));
  radio1->connect("checked", []() {
    Field_content::gameType = 1; // 1 point
  });
  this->group->add(radio1);

  radio2 = tgui::RadioButton::create();
  radio2->setPosition(tgui::bindLeft(radio1) + 120, tgui::bindTop(radio1));
  radio2->connect("checked", []() {
    Field_content::gameType = 3; // turn
  });
  this->group->add(radio2);

  this->group->setPosition(135, "90%");
  this->gui->add(this->group);

  auto label = tgui::Label::create();
  label->setText("normal               no rec"
      "      without points         take turn");
  label->setTextSize(17);
  label->setPosition(tgui::bindLeft(this->group) - 50, tgui::bindTop(this->group) + 20);
  this->gui->add(label);

  Field_content::gameType = 1;
}

void ChooseLayoutState::initGui() {
  this->gui = std::make_unique<tgui::Gui>(*this->window);

  this->initChooseField();
  this->initRandomField();
  this->initFileField();

}

void ChooseLayoutState::initRandomField() {
  auto rand = tgui::Button::create();
  rand->setText("generate random Field");
  rand->setPosition({(this->window->getSize().x - rand->getSize().x) / 2, "66%"});
  if (type == LOCAL || type == SERVER || type == BUILD) {
    rand->connect("pressed", [this]() {
      this->states->push_back(std::make_unique<GamingState>(*this, this->type, this->numPlayers));
    });
  }
  this->gui->add(rand);

}

void ChooseLayoutState::initFileField() {
  this->files = tgui::ComboBox::create();
  this->files->setPosition({(this->window->getSize().x - this->files->getSize().x) / 2, "75%"});
  this->files->setDefaultText("Fields resources");
  std::string path = "../resources/fields";
  for (auto const &entry : std::filesystem::directory_iterator(path)) {
    if (!entry.path().extension().compare(".mine")) {
      this->files->addItem(entry.path().filename().replace_extension("").generic_u8string());
    }
  }

  if (type == LOCAL || type == SERVER || type == BUILD) {
    this->files->connect("ItemSelected", [this]() {
      this->states->push_back(std::make_unique<GamingState>(*this,
                                                            this->files->getSelectedItem(),
                                                            this->type,
                                                            this->numPlayers));
    });
  }

  this->gui->add(files);
}
void ChooseLayoutState::initChooseField() {
  this->width = tgui::Slider::create();
  this->width->setPosition({"25%", "38%"});
  this->width->setMinimum(2);
  this->width->setMaximum(30);
  this->width->setValue(6);
  this->width->setSize({"50%", "12"});
  this->gui->add(this->width);

  this->wLabel = tgui::Label::create();
  this->wLabel->setTextSize(this->width->getSize().y * 1.5);
  this->gui->add(this->wLabel);

  this->height = tgui::Slider::copy(this->width);
  this->height->setPosition({"25%", "46%"});
  this->gui->add(this->height);

  this->hLabel = tgui::Label::copy(this->wLabel);
  this->gui->add(this->hLabel);

  this->numBombs = tgui::Slider::copy(this->height);
  this->numBombs->setPosition({"25%", "54%"});
  this->numBombs->setMinimum(1);
  this->numBombs->setMaximum(35);
  this->gui->add(this->numBombs);

  this->nLabel = tgui::Label::copy(this->hLabel);
  this->gui->add(this->nLabel);

  auto confirm = tgui::Button::create();
  confirm->setText("  confirm\nparameters");
  confirm->setPosition({tgui::bindRight(this->height) + 12, tgui::bindTop(this->height)});
  if (type == LOCAL || type == SERVER || type == BUILD) {
    confirm->connect("pressed", [this]() {
      this->states->push_back(std::make_unique<GamingState>(*this,
                                                            this->width->getValue(),
                                                            this->height->getValue(),
                                                            this->numBombs->getValue(),
                                                            type,
                                                            this->numPlayers));
    });
  }

  this->gui->add(confirm);
}

void ChooseLayoutState::update(float dt) {
  while (this->window->pollEvent(*this->ev)) {
    this->gui->handleEvent(*this->ev);
  }
  this->updateGui();
}

void ChooseLayoutState::updateGui() {
  this->numBombs->setMaximum(this->width->getValue() * this->height->getValue() - 1);
  this->wLabel->setText("width: " + std::to_string((int) this->width->getValue()));
  this->wLabel->setPosition({tgui::bindLeft(this->width) + this->width->getSize().x / 2 - this->wLabel->getSize().x / 2,
                             tgui::bindTop(this->width) + 20});
  this->hLabel->setText("height: " + std::to_string((int) this->height->getValue()));
  this->hLabel->setPosition({tgui::bindLeft(this->height) + this->height->getSize().x / 2
                                 - this->hLabel->getSize().x / 2, tgui::bindTop(this->height) + 20});
  this->nLabel->setText("#bombs: " + std::to_string((int) this->numBombs->getValue()));
  this->nLabel->setPosition({tgui::bindLeft(this->numBombs) + this->numBombs->getSize().x / 2
                                 - this->nLabel->getSize().x / 2, tgui::bindTop(this->numBombs) + 20});
}

void ChooseLayoutState::render() {
  this->gui->draw();
}

