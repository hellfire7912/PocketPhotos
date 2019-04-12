#include <PocketStar.h>
#include <SdFat.h>


SdFat fotos;
SdFile picDir;

long updateTimer = 0;

long picsOnSd = 0;
long cursorPosY = 0;
long picIndex=0;
boolean slideshow = true;

void initialize() {
   pocketstar.setFont(pocketStar6pt);
   pocketstar.setCursor(0, 0);
   pocketstar.setFontColor(GREEN_16b, BLACK_16b);

  if (fotos.begin()){
    fotos.chdir("/");
    if (fotos.exists("pics")) {
        fotos.chdir("/pics");        
      }
         
      while (picDir.openNext(fotos.vwd(), FILE_READ)) {
          if (isValid()) {
              picsOnSd++;
          }
          picDir.close();
      }
    }
    //fotos.vwd()->rewind();
    pocketstar.print(picsOnSd);
    updateTimer = 10000;   
}

void update(float deltaTime) {
	updateTimer += deltaTime * 1000;

	//pocketstar.setCursor(0, 0);
	//pocketstar.print(updateTimer);

	if (slideshow && updateTimer > 10000 && picsOnSd > 0)
	{
		updateTimer = 0;            
		picIndex++;
		pocketstar.drawRect(0, 0, 96, 64, true, BLACK_16b);

		if (picIndex > picsOnSd){
			picIndex=1;
		}
		loadImage();		
	}
	
	if (pocketstar.getButtons()){
				
		if (pocketstar.getButtons(ButtonB)){
			slideshow = false;
			return;
		}
		
		if (pocketstar.getButtons(ButtonA)){
			slideshow = true;
			return;
		}
		
		if (pocketstar.getButtons(ButtonLeft)){
			picIndex--;
			if (picIndex < 1){
				picIndex = picsOnSd;
			}				
		}
		
		if (pocketstar.getButtons(ButtonRight)){
			picIndex++;
			if (picIndex > picsOnSd){
				picIndex = 1;
			}				
		}
		loadImage();
		updateTimer = 0;
	}
}

boolean pause() {
    return true;
}

void resume() {
    loadImage();
  }

void loadImage(){
	long searchIndex=0;
	boolean found = false;

	fotos.vwd()->rewind();

	char path[70];

	while(!found){      
		picDir.close();       
		picDir.openNext(fotos.vwd(), FILE_READ);        
		searchIndex++;
		if (searchIndex >= picIndex){
			found = true;         
		}       
	}

	picDir.getName(path, 32);            
	File file = fotos.open(path, FILE_READ);

	pocketstar.setCursor(0, 0);
	pocketstar.print(picIndex);
	pocketstar.print(" ");
	pocketstar.print(searchIndex);
	pocketstar.print(" ");  

	pocketstar.print(path);

	char s1 = file.read();
	char s2 = file.read();
	char s3 = file.read();    
	long fversion = file.read();
	long width = file.read();
	long height = file.read();

	//  signature: PSI                      version: 2       width and     height: 96 & 64
	if (s1 != 'P' || s2 !='S' || s3 != 'I' || fversion != 2 || width > 96 || height > 64) {
	//if (file.read() != 'P' || file.read() != 'S' || file.read() != 'I' || file.read() != 2 || file.read() != 96 || file.read() != 64) {
		pocketstar.setBitDepth(BitDepth16);
		pocketstar.setFontColor(RED_16b, BLACK_16b);
		pocketstar.drawRect(0, 0, 96, 64, true, BLACK_16b);
		pocketstar.setCursor(0, 0);
		pocketstar.print(path);
		pocketstar.setCursor(0, 10);
		pocketstar.print("unable to read image");

		pocketstar.setCursor(0, 20);
		pocketstar.print("Signature: ");
		pocketstar.print(s1);
		pocketstar.print(s2);
		pocketstar.print(s3);

		pocketstar.setCursor(0, 30);
		pocketstar.print("version: ");
		pocketstar.print(fversion);

		pocketstar.setCursor(0, 40);
		pocketstar.print("size: ");
		pocketstar.print(width);
		pocketstar.print("x");
		pocketstar.print(height);
		return;
	}

	boolean bitDepth16 = file.read() == 0;
	if (bitDepth16)
		pocketstar.setBitDepth(BitDepth16);
	else
		pocketstar.setBitDepth(BitDepth8);

	//int datalines = 64;
	int datalines = height;
	if (bitDepth16)
		datalines = datalines << 1;

	uint8_t buffer[width];
	pocketstar.setX(0, width-1);
	pocketstar.setY(0, height -1);
	pocketstar.startTransfer();
	for (int i = 0; i < datalines; i++) {
		file.read(&buffer, width);
		pocketstar.writeBuffer(buffer, width);
	}
	
	pocketstar.endTransfer();
	file.close();
}
  
  
boolean isValid() {
    
    char path[33];    
    picDir.getName(path, 32);
    
    //pocketstar.setCursor(0, cursorPosY);
    //pocketstar.print(path);
    //cursorPosY += 7;

    if (!fotos.exists(path))
        return false;
        
    return true;
}  