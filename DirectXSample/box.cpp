#include "box.h"

//=============================================================================
// デフォルトコンストラクタ
//=============================================================================
Box::Box() : Entity()
{
	spriteData.width = boxNS::WIDTH;           // 宇宙船1のサイズ
	spriteData.height = boxNS::HEIGHT;
	spriteData.x = boxNS::X;                   // 画面上の位置
	spriteData.y = boxNS::Y;
	spriteData.rect.bottom = boxNS::HEIGHT;    // 画面の一部を選択
	spriteData.rect.right = boxNS::WIDTH;
	fieldX = spriteData.x / boxNS::WIDTH;
	fieldY = spriteData.y / boxNS::HEIGHT;
	oldX = boxNS::X;
	oldY = boxNS::Y;
	oldAngle = 0.0f;
	rotation = 0.0f;
	velocity.x = 0;								// 速度X
	velocity.y = 0;
	frameDelay = boxNS::BOX_ANIMATION_DELAY;
	// 宇宙船アニメーションの最初のフレーム
	startFrame = rand() % (boxNS::BOX_END_FRAME + 1);
	// 宇宙船アニメーションの最後のフレーム
	endFrame = startFrame;
	boxType = startFrame;
	currentFrame = startFrame;
	radius = boxNS::WIDTH / 2.0;
	collisionType = entityNS::CIRCLE;
	direction = boxNS::NONE;                   // 回転の力の方向
	explosionOn = false;
	isGrounded = false;
}

//=============================================================================
// 宇宙船を初期化
// 実行後：成功した場合はtrue、失敗した場合はfalseを戻す
//=============================================================================
bool Box::initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	explosion.initialize(gamePtr->getGraphics(), width, height, ncols, textureM);
	explosion.setFrames(boxNS::EXPLOSION_START_FRAME, boxNS::EXPLOSION_END_FRAME);
	explosion.setCurrentFrame(boxNS::EXPLOSION_START_FRAME);
	explosion.setFrameDelay(boxNS::EXPLOSION_ANIMATION_DELAY);
	explosion.setLoop(false);               // アニメーションをループしない
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// 宇宙船を描画
//=============================================================================
void Box::draw()
{
	if (explosionOn)
		explosion.draw(spriteData);			// 現在のspriteDataを使って爆発を描画
	else
	{
		Image::draw();						// 宇宙船を描画
	}
}

//=============================================================================
// Update
// 通常、フレームごとに1回呼び出す
// frameTimeは、移動とアニメーションの速さを制御するために使用
//=============================================================================
void Box::update(float frameTime, Box* boxInfo[10][10])
{
	if (explosionOn)
	{
		explosion.update(frameTime);
		// 爆発アニメーションが完了した場合
		if (explosion.getAnimationComplete())
		{
			explosionOn = false;                // 爆発をオフ
			visible = false;
			explosion.setAnimationComplete(false);
			explosion.setCurrentFrame(boxNS::EXPLOSION_START_FRAME);
		}
	}

	velocity.y = (float)boxNS::VELOCITY_Y;


	Entity::update(frameTime);
	oldX = spriteData.x;                        // 現在の位置を保存
	oldY = spriteData.y;
	oldAngle = spriteData.angle;
	
	// 落下先が画面外or固定ボックスがあったら落下できない
	if (!isGrounded) {
		spriteData.y += frameTime * velocity.y;
	}
	if (fieldY + 1 >= GAME_HEIGHT / boxNS::HEIGHT || (boxInfo[fieldX][fieldY + 1] != NULL && boxInfo[fieldX][fieldY + 1]->getIsGrounded())) {
		isGrounded = true;
	}
	else {
		isGrounded = false;
	}

	// 箱が一定以上落下したら
	if (spriteData.y >= (fieldY + 1) * boxNS::HEIGHT) {
		// 箱のフィールド上の座標をアップデート
		fieldY += 1;
		// ずれが生じないように念のため位置を修正
		spriteData.y = (fieldY)* boxNS::HEIGHT;
	}

	// 画面の端で回り込む
	if (spriteData.x > GAME_WIDTH)              // 画面右端を超えたら
		spriteData.x = -boxNS::WIDTH;		    // 画面左端に移動
	if (spriteData.x < -boxNS::WIDTH)			// 画面左端を超えたら
		spriteData.x = GAME_WIDTH;				// 画面右端に移動
	if (spriteData.y < -boxNS::HEIGHT)			// 画面上端を超えたら
		spriteData.y = GAME_HEIGHT;				// 画面下端に移動
}

//=============================================================================
// ダメージ
//=============================================================================
void Box::damage(WEAPON weapon)
{

	switch (weapon)
	{
	case PLAYER_ATTACK:
		audio->playCue(TORPEDO_HIT);
		health -= boxNS::PLAYER_DAMEGE;
		break;
	}
	if (health <= 0)
		explode();
}

//=============================================================================
// 爆発
//=============================================================================
void Box::explode()
{
	audio->playCue(EXPLODE);
	active = false;
	health = 0;
	explosionOn = true;
	velocity.x = 0.0f;
	velocity.y = 0.0f;
}

//=============================================================================
// 回復
//=============================================================================
void Box::repair()
{
	active = true;
	health = FULL_HEALTH;
	explosionOn = false;
	rotation = 0.0f;
	direction = boxNS::NONE;           // 回転の力の方向
	visible = true;
}