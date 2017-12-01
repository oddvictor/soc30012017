from SimpleCV import *
import sys 			# Importerer sys for å kunne ta med argumenter inn i programmet

cam = Camera()
img=cam.getImage()
img.save(sys.argv[1]+".jpg") 	#Lagrer bildet til fil med filnavn tatt fra sys.argv[1] og legger til endingen .jpg 