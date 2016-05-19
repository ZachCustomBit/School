//
//  ViewController.swift
//  TicTacToe
//
//  Created by Zach Larsen on 4/20/16.
//  Copyright © 2016 Zach Larsen. All rights reserved.
//

import UIKit

class TicTacToeViewController: UIViewController {
    
    @IBOutlet var ticTacImg1: UIImageView!
    @IBOutlet var ticTacImg2: UIImageView!
    @IBOutlet var ticTacImg3: UIImageView!
    @IBOutlet var ticTacImg4: UIImageView!
    @IBOutlet var ticTacImg5: UIImageView!
    @IBOutlet var ticTacImg6: UIImageView!
    @IBOutlet var ticTacImg7: UIImageView!
    @IBOutlet var ticTacImg8: UIImageView!
    @IBOutlet var ticTacImg9: UIImageView!
    
    @IBOutlet var ticTacBtn1: UIButton!
    @IBOutlet var ticTacBtn2: UIButton!
    @IBOutlet var ticTacBtn3: UIButton!
    @IBOutlet var ticTacBtn4: UIButton!
    @IBOutlet var ticTacBtn5: UIButton!
    @IBOutlet var ticTacBtn6: UIButton!
    @IBOutlet var ticTacBtn7: UIButton!
    @IBOutlet var ticTacBtn8: UIButton!
    @IBOutlet var ticTacBtn9: UIButton!
    
    @IBOutlet var resetBtn: UIButton!
    
    @IBOutlet var userMessage: UILabel!
    
    var plays = Dictionary<Int,Int>()
    var aiDeciding  = false
    var done = false
    
    @IBAction func UIButtonClicked(sender:UIButton) {
        userMessage.hidden = true
        if (plays[sender.tag] == nil && !aiDeciding && !done) {
            setImageForSpot(sender.tag, player:1)
        }
        checkForWin()
        let time = dispatch_time(dispatch_time_t(DISPATCH_TIME_NOW), 1 * Int64(NSEC_PER_SEC))
        dispatch_after(time, dispatch_get_main_queue()) {
            self.aiTurn()
        }
        
        
    }
    
    func setImageForSpot(spot:Int,player:Int) {
        let playerMark = player == 1 ? "x" : "o"
        plays[spot] = player
        switch spot {
        case 1:
            ticTacImg1.image = UIImage(named: playerMark)
        case 2:
            ticTacImg2.image = UIImage(named: playerMark)
        case 3:
            ticTacImg3.image = UIImage(named: playerMark)
        case 4:
            ticTacImg4.image = UIImage(named: playerMark)
        case 5:
            ticTacImg5.image = UIImage(named: playerMark)
        case 6:
            ticTacImg6.image = UIImage(named: playerMark)
        case 7:
            ticTacImg7.image = UIImage(named: playerMark)
        case 8:
            ticTacImg8.image = UIImage(named: playerMark)
        case 9:
            ticTacImg9.image = UIImage(named: playerMark)
        default:
            ticTacImg5.image = UIImage(named: playerMark)
        }
    }
    
    func checkForWin() {
        let whoWon = ["I":0,"you":1]
        for(key,value) in whoWon {
            if((plays[7] == value && plays[8] == value && plays[9] == value) || //across the bottom
                (plays[4] == value && plays[5] == value && plays[6] == value) || //across the middle
                (plays[1] == value && plays[2] == value && plays[3] == value) || //across the top
                (plays[1] == value && plays[4] == value && plays[7] == value) || //down the left
                (plays[2] == value && plays[5] == value && plays[8] == value) || //down the middle
                (plays[3] == value && plays[6] == value && plays[9] == value) || //down the right
                (plays[1] == value && plays[5] == value && plays[9] == value) || //diag left right
                (plays[3] == value && plays[5] == value && plays[7] == value)) { //diag right left
                    userMessage.hidden = false
                    userMessage.text = "Looks like \(key) won!"
                    resetBtn.hidden = false
                    done = true
            }
        }
    }
    
    func checkBottom(value value:Int) -> (location:String,pattern:String) {
        return ("bottom",checkFor(value, inList: [7,8,9]))
    }
    func checkTop(value value:Int) -> (location:String,pattern:String) {
        return ("top",checkFor(value, inList: [1,2,3]))
    }
    func checkLeft(value value:Int) -> (location:String,pattern:String) {
        return ("left",checkFor(value, inList: [1,4,7]))
    }
    func checkMiddleAcross(value value:Int) -> (location:String,pattern:String) {
        return ("middleHorz",checkFor(value, inList: [4,5,6]))
    }
    func checkMiddleDown(value value:Int) -> (location:String,pattern:String) {
        return ("middleVert",checkFor(value, inList: [2,5,8]))
    }
    func checkDiagLeftRight(value value:Int) -> (location:String,pattern:String) {
        return ("diagLeftRight",checkFor(value, inList: [3,5,7]))
    }
    func checkDiagRightLeft(value value:Int) -> (location:String,pattern:String) {
        return ("diagRightLeft",checkFor(value, inList: [1,5,9]))
    }
    
    func checkFor(value:Int, inList:[Int]) -> String {
        var conclusion = ""
        for cell in inList {
            if plays[cell] == value {
                conclusion += "1"
            } else {
                conclusion += "0"
            }
        }
        return conclusion
    }
    
    func rowCheck(value value:Int) -> (location:String,pattern:String)? {
        let acceptableFinds = ["011", "110", "101"]
        let findFuncs = [checkTop, checkBottom, checkLeft, checkMiddleAcross, checkMiddleDown, checkDiagLeftRight, checkDiagRightLeft]
        for algorithm in findFuncs {
            let algorithmResults = algorithm(value:value)
            if (acceptableFinds.contains(algorithmResults.pattern)) {
                return algorithmResults
            }
        }
        return ("a","a")
    }
    
    func isOccupied(spot:Int) -> Bool {
        if (plays[spot] != nil) {
            return true
        } else {
            return false
        }
    }
    
    func aiTurn() {
        if done {
            return
        }
        
        aiDeciding = true
        
        //If the computer has two in a row
        if let result = rowCheck(value:0) {
            let whereToPlayResult = whereToPlay(result.location,pattern: result.pattern)
            if !isOccupied(whereToPlayResult) {
                setImageForSpot(whereToPlayResult, player: 0)
                aiDeciding = false
                checkForWin()
                return
            }
            
        }
        //If the player has two in a row
        if let result = rowCheck(value:1) {
            let whereToPlayResult = whereToPlay(result.location,pattern: result.pattern)
            if !isOccupied(whereToPlayResult) {
                setImageForSpot(whereToPlayResult, player: 0)
                aiDeciding = false
                checkForWin()
                return
            }
            
        }
        // is center available?
        
        if !isOccupied(5){
            setImageForSpot(5, player: 0)
            aiDeciding = false
            checkForWin()
            return
        }
        
        
        
        // is a corner available
        if let cornerAvailable = firstAvailable(isCorner: true) {
            setImageForSpot(cornerAvailable, player: 0)
            aiDeciding = false
            checkForWin()
            return
        }
        
        // is a side available
        if let sideAvailable = firstAvailable(isCorner: false) {
            setImageForSpot(sideAvailable, player: 0)
            aiDeciding = false
            checkForWin()
            return
        }
        
        userMessage.hidden = false
        userMessage.text = "Looks like it was a tie!"
        
        reset()
        
        aiDeciding = false
    }
    
    func firstAvailable(isCorner isCorner:Bool) -> Int? {
        let spots = isCorner ? [1,3,7,9] : [2,4,6,8]
        for spot in spots {
            if !isOccupied(spot) {
                return spot
            }
        }
        return nil
    }
    
    func whereToPlay(location:String,pattern:String) -> Int {
        let leftPattern = "011"
        let rightPattern = "110"
        _ = "101"
        
        switch location {
        case "top":
            if pattern == leftPattern {
                return 1
            } else if pattern == rightPattern {
                return 3
            } else {
                return 2
            }
        case "bottom":
            if pattern == leftPattern {
                return 7
            } else if pattern == rightPattern {
                return 9
            } else {
                return 8
            }
        case "left":
            if pattern == leftPattern {
                return 1
            } else if pattern == rightPattern {
                return 7
            } else {
                return 4
            }
        case "right":
            if pattern == leftPattern {
                return 3
            } else if pattern == rightPattern {
                return 9
            } else {
                return 6
            }
        case "middleVert":
            if pattern == leftPattern {
                return 2
            } else if pattern == rightPattern {
                return 8
            } else {
                return 5
            }
        case "middleHort":
            if pattern == leftPattern {
                return 4
            } else if pattern == rightPattern {
                return 6
            } else {
                return 5
            }
        case "diagRightLeft":
            if pattern == leftPattern {
                return 3
            } else if pattern == rightPattern {
                return 7
            } else {
                return 5
            }
        case "diagLeftRight":
            if pattern == leftPattern {
                return 1
            } else if pattern == rightPattern {
                return 9
            } else {
                return 5
            }
        default: return 4
        }
    }
    
    @IBAction func resetBtnClicked(sender:UIButton) {
        done = false
        resetBtn.hidden = true
        userMessage.hidden = true
        reset()
    }
    
    func reset() {
        plays = [:]
        ticTacImg1.image = nil
        ticTacImg2.image = nil
        ticTacImg3.image = nil
        ticTacImg4.image = nil
        ticTacImg5.image = nil
        ticTacImg6.image = nil
        ticTacImg7.image = nil
        ticTacImg8.image = nil
        ticTacImg9.image = nil
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.navigationController?.navigationBarHidden = false
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func prefersStatusBarHidden() -> Bool {
        return true
    }
}
